basename <- "article"
all_methods <- c("Random","Square","Grid","Peri","Quad","OldEPI","NewEPI","EPI3","EPI5", "SA", "EPI")

newnames <- c(`strip_epi-s1`="OldEPI strip",
              `arc_epi-s1`="OldEPI arc",`strip_epi-s3`="EPI3 strip",`arc_epi-s3`="EPI3 arc",
              `strip_epi-s5`="EPI5 strip",`arc_epi-s5`="EPI5 arc",
              peri_epi="Peri",qtr_epi="Quad",grid_epi="Grid",circle_gps="Circle",square_gps="Square",
              enum="NewEPI",
              SA="SA", EPI = "EPI")

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


