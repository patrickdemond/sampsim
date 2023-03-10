## SIMRESULTS=run1and2REV.xlsx
SIMRESULTS=Results4x50.xlsx

all: figs

figs: article_final.rda article_final_setup.R final_fig_funs.R
	Rscript --vanilla article_final.R

## arg 1: file name containing results (xlsx)
## arg 2: number of tables/simulation methods run
article_final.rda: article_final_setup.R final_setup_funs.R
	Rscript --vanilla article_final_setup.R --args $(SIMRESULTS) 4

clean:
	rm -f article_final_fig*.pdf article_*.zip

