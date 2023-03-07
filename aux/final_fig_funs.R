library(readxl)
library(readr)
library(tidyr)
library(dplyr)
library(stringr)
library(directlabels)
library(ggplot2); theme_set(theme_bw())
library(colorspace)
library(cowplot)
library(ggrepel)
library(ggforce) ## facet_wrap_paginate

basename <- "article"

newnames <- c(`strip_epi-s1`="OldEPI strip",
              `arc_epi-s1`="OldEPI arc",`strip_epi-s3`="EPI3 strip",`arc_epi-s3`="EPI3 arc",
              `strip_epi-s5`="EPI5 strip",`arc_epi-s5`="EPI5 arc",
              peri_epi="Peri",qtr_epi="Quad",grid_epi="Grid",circle_gps="Circle",square_gps="Square",
              enum="NewEPI")

## set default colour palette??
##scale_colour_discrete <- function(...) {
##    colorspace::scale_colour_discrete_qualitative(...)
## }

## general strategy from https://debruine.github.io/posts/multi-row-headers/:
## (1) read header lines as data frame; (2) transpose; (3) na.locf;
## (4) paste together (then ready to gather/pivot_longer)
#' @param fn file name
#' @param skip how many rows to skip
#' @param n_header number of header rows
#' @param n_table number of data rows
#' @param read_fun function to read
#' @param extra_names character vector of prepended column names
#' @param drop_hdr_rows number of header rows to drop
#' @param ... extra arguments to read_fun()
read_with_header <- function(fn,skip=0,n_header,n_table,
                             read_fun=read_excel,
                             extra_names=NULL,
                             drop_hdr_rows=0,
                             ...) {
    cn <- (
        read_fun(fn,skip=skip,n_max=n_header,col_names=FALSE,
                 ...) 
        %>% t()
        %>% as_tibble(.name_repair="unique")
        %>% mutate_at("...2",~gsub("_",".",.)) ## fix underscores found in names
    )
    if (drop_hdr_rows>0) {
        cn <- cn[-(1:drop_hdr_rows),]
    }
    cn <- (cn
        %>% mutate_all(zoo::na.locf)
        %>% apply(MARGIN=1,paste,collapse="_")
    )
    main <- (
        read_fun(fn,skip=skip+n_header,
                 n_max=n_table,
                 col_names=FALSE, ...)
        %>% setNames(c(extra_names,cn))
    )
    return(main)
}

#' @param RR relative risk (i.e. which sheet to read
#' @inheritParams read_with_header
#' @param nsim number of sims (for computing standard error)
#' @param h_rows we may want different subsets of response/n/stat
#' @param resamp_data get data from town resampling
#' @param fn file name
#' @param n_table number of data rows
get_MSE_tab <- function(RR,
                        n_table=12,
                        skip=n_table+n_header+1,
                        n_header=3,
                        h_rows=seq(n_header),
                        resamp_data=FALSE,
                        fn="run1.20190906.xlsx",
                        nsim=50, extra_names="method",
                        sub_names=TRUE,
                        debug=FALSE,...) {
    sheetname <- sprintf("RR=%1.1f",RR)
    sheetname <- gsub("\\.0$","",sheetname)
    if (debug) cat("sheetname: ",sheetname,"\n")
    if (resamp_data) sheetname <- paste(sheetname,"(town resample)")
    mr2 <- read_with_header(fn,skip=skip, n_header=n_header,n_table=n_table,
                           extra_names=extra_names, sheet=sheetname,
                           ...)
    mr2 <- mr2[!is.na(names(mr2))] ## ??? why necessary ???
    names_to <- c("response","n","stat")[h_rows]
    names_pattern <- paste(c("(.*)",
                             ## would like to use parentheses to
                             ## delimit n= but that makes another pattern
                             "n?=?([0-9]+)",
                             ## include dot as well as alpha in var names
                             "([[:alpha:].]+)")[h_rows],
                           collapse="_")
    names_ptypes <- list(response=character(0),
                         n=character(0),
                         stat=character(0))[h_rows]
    mr2 <- (mr2
        %>% pivot_longer(-one_of(extra_names),
                         values_to="MSE_ratio",
                         names_to=names_to,
                         names_pattern=names_pattern,
                         names_ptypes=names_ptypes)
        ## ?? needed with new vctrs ?? 0.3.0 ??
        %>% mutate_at("n",as.integer)
    )
    if ("stat" %in% names(mr2)) {
        mr2 <- (mr2
            %>% pivot_wider(names_from="stat",values_from="MSE_ratio")
            %>% mutate(ymin=mean-stdev/sqrt(nsim),ymax=mean+stdev/sqrt(nsim))
        )
    }
    if (sub_names) {
        mr2 <- mr2 %>%
            mutate(method=factor(method,levels=unique(method),
                                 labels=newnames[match(as.character(unique(method)),names(newnames))]))
    }
    return(mr2)
}


##' iterate over rel risk values
##' @param RRvals vector of relative-risk values
get_all_MSEtab <- function(RRvals=c(1,1.5,2,3),...) {
    x1 <- suppressMessages(purrr::map(RRvals,get_MSE_tab,...)
        %>% setNames(RRvals)
        %>% bind_rows(.id="RR")
    )
    x2 <- mutate(x1, RR=as.numeric(RR))
    return(x2)
}

get_partabs <- function(npops=50,
                        fn="run1.20190906.xlsx",
                        n_table=12,
                        n_hdr=3,
                        RR = 1,
                        resamp_data=FALSE,
                        n_vanilla=3,
                        progress = TRUE,
                        ...) {
    resList <- list()
    pop_data <- read_excel(fn,sheet="populations")
    if (progress) pb <- txtProgressBar(style = 3, max = npops)
    for (i in 1:npops) {
        ## cat(i,"\n")  ## real progress bar??
        if (progress) setTxtProgressBar(pb, i)
        ## 5 tables to skip (means of ratios; prevalence; rank 7, 15, 30)
        ##  5*(n_table+3+1)+1
        skip_rows <- (n_table+n_hdr+1)*5+1
        tabs <- (n_table+1)+n_hdr+1
        suppressMessages(resList[[i]] <-
                        get_MSE_tab(skip=skip_rows+(i-1)*tabs,  ## explicit row calculation
                        RR=RR,
                        resamp_data=resamp_data,
                        n_header=2,h_rows=2:3,
                        drop_hdr_rows=2,
                        n_table=n_table+1, ## includes random!
                        extra_names=c("method","true_mean","true_rr"),
                        na=c("","-nan","nan","NA","n/a"),
                        fn=fn,
                        ...)
                )
    }
    if (progress) close(pb)
    ## combine ...
    nmvec <- c(0:(npops-n_vanilla-1),paste0("vanilla-",1:n_vanilla))
    res <- (resList
        %>% setNames(nmvec)
        %>% bind_rows(.id="population")
        %>% full_join(pop_data,by="population")
        ## reorder method to match mr (ordered by MSE)
        %>% filter(method!="random")
        ## %>% mutate(method=factor(method,levels=levels(mr2$method)))
    )
    return(res)
}

x_vars <- c("dweight_age",
          "dweight_income",
          "dweight_risk",
          "mean_household_pop",
          "disease_pockets",
          "pocket_scaling",
          "popdens_mx",
          "popdens_my",
          "true_mean",
          "p_ratio")

panel_names <- c("weight for age",
                 "weight for income",
                 "weight for risk",
                 "mean household population",
                 "number of disease pockets",
                 "scaling [impact] of pockets",
                 "population density gradient [x-axis]",
                 "population density gradient [y-axis]",
                 "true population prevalence",
                 "disease probability ratio")


all_methods <- c("Random","Square","Grid","Peri","Quad","OldEPI","NewEPI","EPI3","EPI5")

mkfig <- function(focal_n=7, focal_RR=1, focal_resamp=TRUE, response="prev",
                  focal_var=NULL,
                  exp_lim=0,
                  minval=NA,
                  point_alpha=0.05,
                  legend_pos=c(0.65,0.15),
                  use_ribbons=FALSE,
                  do_shape=FALSE,
                  mlevs=setdiff(all_methods,exclude_methods),
                  iwh_vec=setNames(c("#c77533","#8a66d3","#86a83c","#c85998","#60a16c",
                                     "#897bbf","#c8615d","#49afcf"), mlevs),
                  col_vec=setNames(c("#000000",iwh_vec),mlevs),
                  shape_vec=setNames(c(3,  ## random
                                          16:19, ## non-EPI
                                          21:24), ## EPI
                                        mlevs),
                  exclude_methods=c("EPI3","Grid"),
                  lty_vec=c("solid","11")[1+as.numeric(grepl("epi",mlevs,ignore.case=TRUE))]
                  ) {
    size_name <- paste0("true\n", if (response=="prev") "mean\nprevalence" else "relative\nrisk")
    resamp_str <- if (focal_resamp) "resample" else "no_resample"
    fn <- sprintf("%s_final_fig_SUPP_2_n%d_%s_%s_RR%1.1f.pdf",
                  basename,nval,resamp_str,response,focal_RR)
    cat(fn,"\n")
    ## should be: n (3) x RR (4) x resamp (2) x response (2)
    plot_data <- (gg
                    %>% filter(n==focal_n,RR==focal_RR,resamp==focal_resamp)
                    %>% mutate(focal_mse=if (response=="prev") mse else mse.rr,
                               true_val=(if (response=="prev") true_mean else true_rr))
                    %>% filter(!method %in%  exclude_methods)
                    %>% mutate(rmse=sqrt(focal_mse)
                             , method=factor(method,levels=mlevs)
                             , epi=grepl("EPI",method))
                )
                plot_data_long <- (plot_data
                    %>% select(all_of(c("rmse","method","epi","true_val",x_vars)))
                    %>% pivot_longer(cols=all_of(x_vars),names_to="var",values_to="value")
                    %>% mutate_at("var",factor,levels=x_vars, labels=panel_names)
                )
                if (!is.null(focal_var)) {
                    fv_name <- panel_names[match(focal_var,x_vars)]
                    plot_data_long <- filter(plot_data_long, var==fv_name)
                }
                gg1 <- (ggplot(plot_data_long, aes(value, rmse,
                                         colour=method,
                                         label=method)
                               )
                    + scale_x_log10()
                    + scale_y_log10(limits=c(minval,NA), oob=scales::squish)
                    + expand_limits(x=exp_lim) ## make room for direct labels
                    + labs(y="Root mean squared error")
                    + scale_colour_manual(values=col_vec,
                                          guide=guide_legend(reverse=TRUE)
                                          )
                )
                if (is.null(focal_var)) {
                    gg1 <- gg1 + facet_wrap(~var,scale="free_x") + labs(x="")
                } else {
                    gg1 <- gg1 + labs(x=fv_name)
                }
                if (do_shape) {
                    gg2 <- (gg1
                        + geom_point(alpha=point_alpha,aes(size=true_val,shape=method))
                        + scale_size(name=size_name)
                        + scale_shape_manual(values=shape_vec,guide=FALSE) 
                    )
                } else {
                    gg2 <- (gg1
                        + geom_point(alpha=point_alpha,aes(size=true_val))
                        + scale_size(name=size_name)
                    )
                }
                gg2S <- (gg2 
                    + geom_smooth(alpha=0.1,span=0.9, se=use_ribbons,
                                  ## data=subset(data,p_ratio>1),
                                  aes(linetype=method),
                                  method="loess",
                                  formula=y~x)
                    + scale_linetype_manual(values=lty_vec,
                                            guide=guide_legend(reverse=TRUE)
                                            )
                )
                nval <- as.integer(as.character(focal_n))
                if (is.null(focal_var)) {
                    gg3 <- (gg2S
                        + theme(legend.position=legend_pos,legend.box="horizontal")
                        + ggtitle(sprintf("n=%d: %s: %s: RR=%1.1f",nval, resamp_str, response, focal_RR))
                    )
                } else {
                    gg3 <- gg2S
                }

}
