library(FSelector)
library(ggplot2)
source("experimentAPI.R")

EDGE_TYPE_FILE = "../data/semmed.edgetypes" # Example : "../data/lobbyist.edgetypes"
mapfile <- read.csv(EDGE_TYPE_FILE, sep="\t", header=F)
mapfile$V1 <- as.numeric(mapfile$V1)
mapfile$V2 <- as.character(mapfile$V2)

dat <- read.csv("../result/aapp_dsyn//aapp_dsyn.test.csv")

# Change path name
colnames(dat) <- str_replace_all(str_replace(colnames(dat), "X",""), "\\.",",")

# Get weights of raw features

weights <- information.gain(label~., dat)

features <- data.frame(importance=weights$attr_importance, path=row.names(weights))

features <- features[order(-features$importance),]
features$path <- idpath2title(features$path, mapfile)

head(features)
