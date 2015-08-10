library(FSelector)
library(ggplot2)
source("experimentAPI.R")
dat <- read.csv("../result/city/city_capital_all.test.csv")
output_graph <- "city_capital"

# Get weights of raw features

weights <- information.gain(label~., dat)

res <- list()

for(i in seq(0.01,1,by=0.01)) {
  set.seed(233)
  tmp_res <- eval.df(dat[,c("label",cutoff.k.percent(weights,i))])
  auc_score <- as.numeric(str_split(str_split(tmp_res$eval$string,"\n")[[1]][26], " +")[[1]][9])
  res[[length(res)+1]] <- data.frame(auc=auc_score, cutoff = i)
  print(res[[length(res)]])
}

res.df <- rbind.fill(res)

set.seed(233)
max_res <- eval.df(dat[,c("label",cutoff.biggest.diff(weights))])
max_auc <- as.numeric(str_split(str_split(max_res$eval$string,"\n")[[1]][26], " +")[[1]][9])

print(paste("max auc is ",max(max_auc, res.df$auc)))

g <- ggplot(res.df, aes(x=cutoff,y=auc)) + 
  geom_point() + geom_line() +
  geom_hline(yintercept= max_auc, color="red") +
  ylab("Area Under ROC") +
  xlab("Percentage of Selected Features") +
  scale_x_continuous(expand=c(0,0)) +
  scale_y_continuous(expand=c(0,0), limits=c(0,1)) +
  theme_classic() +
  theme(panel.background = element_rect(colour = "black", size=1))

ggsave(g, filename = paste("../result/feature_selection/", output_graph, ".eps", sep = ""), width = 5, height = 5)