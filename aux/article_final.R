## three vanilla populations (three different prevalences) that had no variability in background risk across the towns.  As expected, the methods were pretty much equivalent.  We also calculated the ratio between the highest and lowest background prevalences for all the populations as a measure of spatial correlation.  I want to include something on that in the

## Would you produce the graphs as before with that ratio as the x-variable?  My hypothesis is that the lines will diverge more as the ratio increases.  If so the practical implication is that when there is a fair bit of spatial correlation you should use the Square (or Circle) method.  If there is only a little correlation, so the statistical advantage of the Square approach is smaller, then other logistical issues need to be considered.  We can note that in general, for prevalence estimation, the New EPI method is better than the OldEPI and most of its variants, but not the best approach available.

## I’m attaching the file that Patrick sent me.  You can see the ratio variable in column W in the Populations sheet.  And there are the results for the three extra population

## The graphs we need are:
## 1.      Graphs of Mean Ratios of RMSE, similar to Fig 2 in CIHR report, but with just the eight methods we’re including.
## We want them for Prevalences and RRs, for all four RRs.

##
## BMB: clean up and re-automate figure 2
##    run get_partabs with RR = 1,2, 3, then
##      select mse.rr


##  2.      Graphs as in Fig 3 in CIHR report, but separate graphs for each sample size:
## RMSE vs. Various parameters, the same ones as before and especially Prevalence ratio – want these for all 8 sheets in the Excel file, i.e., for all RR/Town sampling combinations.
## I expect the paper will just include the graphs for RR=1 using same towns for the simulations. Maybe RR=3 if any of them are at least somewhat different. The others will go into the Supplementary material.


## from article_final_setup.R
library(tidyverse)
theme_set(theme_bw())
library(directlabels)
source("final_fig_funs.R")
load("article_final.rda")

## system("rm -f article_final_fig_*.pdf")

## Figure 1: RMSE vs sample size
exclude_method <- c("EPI3","Grid")
data <- all_MSE
plot_size <- list(width=7,height=5)
for (resp in c("Prevalence", "RR")) {
        for (focal_RR in unique(data$RR)) {
            for (do_resample in c(TRUE,FALSE)) {
                resamp_str <- if (do_resample) "resample" else "no_resample"
                fn <- sprintf("article_final_fig_SUPP_1_%s_RR%1.1f_%s.pdf",resp,focal_RR,resamp_str)

                cat(fn,"\n")

                plot_data <- filter(data,response==resp,RR==focal_RR,
                                    resample==do_resample,
                                    !method %in% exclude_method)
                
                g1 <- (ggplot(plot_data,aes(n,mean,colour=method))
                    + geom_point()
                    + geom_line()
                    + geom_ribbon(aes(ymin=ymin,ymax=ymax,fill=method),
                                  colour=NA,alpha=0.1)
                    + scale_x_continuous(breaks=unique(plot_data$n))
                    + scale_y_log10()
                    + colorspace::scale_colour_discrete_qualitative()
                    + labs(x="sample size per cluster",y="relative RMSE")
                    + geom_dl(aes(label=method),method=list(dl.trans(x = x + .3),cex=0.9,"last.bumpup"))
                    + theme(legend.position="none")
                    + expand_limits(x=32)
                    + ggtitle(sprintf("%s: RR=%1.1f",resp,focal_RR))
                )

                with(plot_size,ggsave(plot=g1,filename=fn,width=width,height=height))
            } ## do_resample
        } ## focal_RR
} ## resp


exclude_methods <- c("EPI3","Grid")
plot_size <- list(width=10,height=8)
exp_lim <- 0
minval <- NA
point_alpha <- 0.05
legend_pos <- c(0.65,0.15)
use_ribbons <- FALSE
do_shape <- FALSE
iwh_vec <- c("#c77533","#8a66d3","#86a83c","#c85998","#60a16c","#897bbf","#c8615d","#49afcf")
col_vec <- c("#000000",iwh_vec)
shape_vec <- c(3,  ## random
               16:19, ## non-EPI
               21:24) ## EPI
mlevs <- c("Random","Square","Grid","Peri","Quad","OldEPI","NewEPI","EPI3","EPI5")
names(col_vec) <- mlevs
names(shape_vec) <- mlevs

mlevs <- setdiff(mlevs,exclude_methods)
col_vec <- col_vec[mlevs]
shape_vec <- shape_vec[mlevs]
lty_vec <- c("solid","11")[1+as.numeric(grepl("epi",mlevs,ignore.case=TRUE))]

plot_list <- list()
for (focal_n in unique(gg$n)) {
    for (focal_RR in unique(gg$RR)) {
        for (focal_resamp in c(TRUE,FALSE)) {
            for (response in c("prev", "rr")) {
                focal_var <- NULL
                nval <- as.integer(as.character(focal_n))
                resamp_str <- if (focal_resamp) "resample" else "no_resample"
                fn <- sprintf("article_final_fig_SUPP_2_n%d_%s_%s_RR%1.1f.pdf",nval,resamp_str,response,focal_RR)
                ## COMMENT ME OUT!
                ## focal_n <- 7; focal_RR <- 1; focal_resamp <- TRUE; response <- "prev"
                ## focal_var <- "p_ratio"
                gg3 <- mkfig(focal_n, focal_RR, focal_resamp, response, focal_var=NULL)
                plot_list[[fn]] <- gg3
                with(plot_size,ggsave(plot=gg3,filename=fn,height=height,width=width))
            }
        }
    }
}

if (FALSE) {
blowup_fn <- "article_final_fig_2_n30_no_resample_rr_RR3.0.pdf"
    gg4 <- mkfig(focal_n=30, focal_RR=3.0, focal_resamp=FALSE, response="prev", focal_var="p_ratio",
                 point_alpha=0.1)
    
    g0 <- ggplot_build(gg4)$data
    d_lens <- sapply(g0,nrow)
    w <- which(d_lens>1 & d_lens!=nrow(data))
    smooth_dat <- g0[[w]]

    ## matplot(unique(smooth_dat$x),matrix(smooth_dat$y,ncol=9))
    ## labpos <- ggmap::gglocator(9)
    labpos <- structure(list(x = c(0.912812799756186, 1.22547836606249, 1.89027214296618, 
                                   1.17083778165944, 0.135702266024018, 1.11316160923401, 1.17690895770423, 
                                   1.00995161647271, 1.21030042595053), y = c(-1.62777596360761, 
                                                                              -1.58815730164323, -1.56485185034025, -1.50775240803093, -1.38888760585497, 
                                                                              -1.42268313117252, -1.56019072887949, -1.45997417030871, -1.46347017375794
                                                                              )), row.names = c(NA, -9L), class = "data.frame")

    ## random =1 ,
    labpos2 <- data.frame(labpos, method=all_methods)

    gg5 <- gg4 + geom_label(data=labpos2,aes(x=10^x,y=10^y,label=method,colour=method),show.legend=FALSE,cex=2,
                            label.padding = unit(0.15, "lines"))

}
## not finished ...
## ggsave("article_fig_12may2020_noshapes.pdf",height=6,width=6)

## new figure 1

### 
exclude_method <- c("EPI3","Grid")
data <- all_MSE
plot_size <- list(width=7,height=5)
focal_RR <- 1.0
do_resample <- FALSE
plot_data <- filter(data,
                    ## response==resp,
                    RR==focal_RR,
                    resample==do_resample,
                    !method %in% exclude_method) %>%
    mutate_at("response",
              ~factor(.,
                      levels=c("Prevalence","RR"),
                      labels=c("(a) estimating prevalence",
                               "(b) estimating relative risk")))
g1 <- (ggplot(plot_data,aes(n,mean,colour=method))
    + geom_point()
    + geom_line()
    + geom_ribbon(aes(ymin=ymin,ymax=ymax,fill=method),
                  colour=NA,alpha=0.1)
    + scale_x_continuous(breaks=unique(plot_data$n))
    + scale_y_log10()
    + colorspace::scale_colour_discrete_qualitative()
    + labs(x="Sample size per cluster",y="Mean RMSE ratio")
    + geom_dl(aes(label=method),method=list(dl.trans(x = x + .3),cex=0.9,"last.bumpup"))
    + theme(legend.position="none")
    + expand_limits(x=36)
    + facet_wrap(~response)
    + theme(panel.spacing=grid::unit(0,"lines"))
)


fn <- "article_final_fig_1.pdf"
with(plot_size,ggsave(plot=g1,filename=fn,width=width,height=height))

## FIGURE 2

exclude_methods <- c("EPI3","Grid")
focal_RR <- 1.0
focal_resamp <- FALSE
mlevs <- setdiff(all_methods,exclude_methods)
iwh_vec <- c("#c77533","#8a66d3","#86a83c","#c85998","#60a16c",
                      "#897bbf","#c8615d","#49afcf")
shape_vec <- setNames(c(3,  ## random
                        16:19, ## non-EPI
                        21:24), ## EPI
                      mlevs)

plot_data <- (gg
    %>% filter(RR==focal_RR,resamp==focal_resamp)
    %>% filter(!method %in%  exclude_methods)
    %>% mutate(method=factor(method,levels=mlevs)
             , epi=grepl("EPI",method))
)

plot_data_long <- (plot_data
    %>% select(all_of(c("mse","mse.rr","true_mean",
                        "method","epi","n")))
    %>% pivot_longer(cols=starts_with("mse"))
    %>% mutate_at("name",
                  ~ case_when(. == "mse" ~ "prevalence",
                              . == "mse.rr" ~ "relative risk"))
    %>% rename(response="name")
    %>% mutate(rmse = sqrt(value))
)

minval <- NA
point_alpha <- 0.2
use_ribbons <- FALSE

gg1 <- (ggplot(plot_data_long, aes(true_mean, rmse,
                                   colour=method,
                                   label=method))
    + scale_x_log10()
    + scale_y_log10(limits=c(minval,NA), oob=scales::squish)
    + scale_colour_manual(values=col_vec,
                          guide=guide_legend(reverse=TRUE)
                          )
    + facet_grid(response~n,scale="free_y",
                 labeller =labeller(n=function(s) 
                     sprintf("n=%s per cluster",s),
                     response=function(s)
                         sprintf("RMSE when\nestimating\n%s",s)),
                 switch="y")
    ## 
    + theme(strip.placement = "outside",
            strip.text.y.left=element_text(angle=0),
            panel.spacing=grid::unit(0,"lines"))
    + labs(x="True prevalence for each population",
           y="")
    + geom_point(alpha=point_alpha)
    + geom_smooth(alpha=0.1,span=0.9, se=use_ribbons,
                  ## data=subset(data,p_ratio>1),
                  aes(linetype=method),
                  method="loess",
                  formula=y~x)
    + scale_linetype_manual(values=lty_vec,
                            guide=guide_legend(reverse=TRUE)
                            )
)

fn <- "article_final_fig_2.pdf"
with(plot_size,ggsave(plot=gg1,filename=fn,width=width,height=height))

zipname <- sprintf("article_final_figs_%s.zip",format(Sys.time(),"%Y%m%d"))
system(sprintf("zip  %s article_final_fig_*.pdf", zipname))
if (interactive()) {
    cat("copying to ms\n")
    system(sprintf("scp %s ms.mcmaster.ca:~/public_html/misc/",zipname))
}
