ca <- commandArgs(trailingOnly=TRUE)[-1]
fn <- ca[1]
ntab <- if (length(ca)<2) 8 else as.numeric(ca[2])
## fn <- "Results4x50.xlsx"
print(fn)
library(tidyverse)
source("final_fig_funs.R")

new_MSE <- get_all_MSEtab(fn=fn,
                          n_table=ntab, debug=TRUE,
                          sub_names=FALSE)

new_MSE_r <- get_all_MSEtab(fn=fn,
                          n_table=ntab, debug=TRUE,
                          sub_names=FALSE,
                          resamp_data=TRUE)

all_MSE <- (bind_rows(list("FALSE"=new_MSE,"TRUE"=new_MSE_r),
                      .id="resample")
    %>% mutate_at("resample", as.logical)
)


RR_vec <- unique(new_MSE$RR)
## debug(get_partabs)
gg <- (map_dfr(c(TRUE,FALSE),
              function(resamp) {
                  map_dfr(setNames(RR_vec,RR_vec),
                          ~get_partabs(npops=53,
                                       fn=fn,
                                       sub_names=FALSE,
                                       n_table=ntab,
                                       resamp_data=resamp,
                                       RR=.),
                          .id="RR")
              },
              .id="resamp")
    %>% mutate_at("resamp",~.=="1")
    %>% mutate_at("RR",as.numeric)
    %>% rename(p_ratio="Prob-ratio")
    %>% mutate_at("n",factor)
    %>% mutate_at("pocket_scaling", ~suppressWarnings(as.numeric(.)))  ## drop 'n/a' - should do upstream
)

## checking code: we do seem to have vanilla1 here ..
## gg %>% filter(RR==1,population=="vanilla-1", resamp==FALSE, n==7) %>% select(method,mse,p_ratio)

save("all_MSE","gg", file="article_final.rda")
