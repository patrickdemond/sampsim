library(directlabels)
library(ggplot2); theme_set(theme_bw())
library(colorspace)
library(cowplot)
library(ggrepel)
library(ggforce) ## facet_wrap_paginate
library(tidyr)
library(dplyr)

source("final_names.R")

## colours retrieved from I Want Hue
iwh_vec <- c("#c77533","#8a66d3","#86a83c","#c85998","#60a16c",
             "#897bbf","#c8615d","#49afcf")
col_vec <- c("#000000", iwh_vec)
    
load("article_final.rda")
## note 'gg' is in GLOBAL environment (ugh)
mkfig <- function(focal_n=7,
                  focal_RR=1,
                  focal_resamp=TRUE,
                  response="prev",
                  focal_var=NULL,
                  exp_lim=0,
                  minval=NA,
                  point_alpha=0.05,
                  legend_pos=c(0.65,0.15),
                  use_ribbons=FALSE,
                  do_shape=FALSE,
                  exclude_methods=c("EPI3","Grid")
                  ) {

    mlevs <- setdiff(unique(gg$method), exclude_methods)
    lty_vec <- c("solid","11")[1+as.numeric(grepl("epi",mlevs,ignore.case=TRUE))]
    nonEPI <- !grepl("EPI", mlevs)
    EPI <- grepl("EPI", mlevs)
    shape_vec <- setNames(rep(NA_integer_, length(mlevs)), mlevs)
    shape_vec[shape_vec == "Random"] <- 3L
    shape_vec[!grepl("EPI", mlevs)] <- 15 + seq(sum(!grepl("EPI", mlevs)))  ## solid
    shape_vec[grepl("EPI", mlevs)]  <- 20 + seq(sum(grepl("EPI", mlevs))) ## filled
    col_vec <- setNames(col_vec[seq_along(mlevs)], mlevs)

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
