library(ggplot2)
dat <- read.csv("../result/empirical_result_f1")
dat$test <- str_replace(dat$test, " ","\n")
g <- ggplot(dat, aes(x=nfeature, y=f1, shape=type, color=type)) + 
  geom_point(size=5) + 
  scale_shape_manual(values=c(15,0,16,1)) +
  scale_x_log10(expand=c(0.1,0)) +
  scale_y_continuous(expand=c(0.1,0.1), limits=c(0.6,1)) +
  ylab("F-Measure") +
  xlab("Number of Features") +
  theme_classic() +
  facet_grid(test~.) + 
  theme(panel.background = element_rect(colour = "black", size=1),
        legend.title=element_blank(),
        legend.position="none",
        legend.background = element_rect(fill = "transparent"),
        legend.text = element_text(size = 8),
        strip.text.y = element_text(size = 8))
  
ggsave("../result/dbpedia_metapath_predicate_fmeasure.eps", g, width = 5, height = 5)

dat <- read.csv("../result/empirical_result_roc")
dat$test <- str_replace(dat$test, " ","\n")
g <- ggplot(dat, aes(x=nfeature, y=roc, shape=type, color=type)) + 
  geom_point(size=5) + 
  scale_shape_manual(values=c(15,0,16,1)) +
  scale_x_log10(expand=c(0.1,0)) +
  scale_y_continuous(expand=c(0.1,0.1), limits=c(0.6,1)) +
  ylab("AUROC") +
  xlab("Number of Features") +
  theme_classic() +
  facet_grid(test~.) +
  theme(panel.background = element_rect(colour = "black", size=1),
        legend.title=element_blank(),
        legend.position="none",
        legend.background = element_rect(fill = "transparent"),
        legend.text = element_text(size = 8),
        strip.text.y = element_text(size = 8))
ggsave("../result/dbpedia_metapath_predicate_roc.eps", g, width = 5, height = 5)

