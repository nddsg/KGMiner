# ---- Cleanup everything before start ----
rm(list = ls())
gc()

# ---- GBSERVER API ----
source("./experimentAPI.R")

# ---- INPUT and CONFIGURATIONS ----

EDGE_TYPE_FILE = "../data/lobbyist.edgetypes" # Example : "../data/lobbyist.edgetypes"
INPUT_FILE = "../facts/lobbyist/firm_official.csv" # Example : "../facts/lobbyist/firm_payee.csv" col 1 and 2 are ids and 3 is label
CLUSTER_SIZE = 48 # Number of workers in gbserver
FALSE_PER_TRUE = 1
DISCARD_REL = 1
ASSOCIATE_REL = c(1,2,3)
OUTPUT_PREFIX = "template/template" # "result/"+OUTPUT_PREFIX


# ---- Load edge type file ----

mapfile <- read.csv(EDGE_TYPE_FILE, sep="\t", header=F)
mapfile$V1 <- as.numeric(mapfile$V1)
mapfile$V2 <- as.character(mapfile$V2)

# ---- Init workers ----

cl <- makeCluster(CLUSTER_SIZE) 
clusterExport(cl = cl, varlist=c("adamic_adar", "semantic_proximity", "ppagerank",
                                 "preferential_attachment", "katz",
                                 "multidimensional_adamic_adar", "heterogeneous_adamic_adar",
                                 "connectedby", "rel_path", "truelabeled", "falselabeled", "str_split",
                                 "as.numeric", "request","DISCARD_REL"), envir = environment())

# ---- Load input data ----
dat.true <- unique(read.csv(INPUT_FILE))

if (ncol(dat.true) < 3)
  dat.true$label <- T

# ---- Construct false labeled data -----
set.seed(233)

# TODO: reformat this so it is universal and file independent
dat.false <- rbind.fill(apply(dat.true, 1, function(x){
  candidates <- unique(dat.true[which(dat.true[,1] != x[1]), 2])
  candidates <- unlist(lapply(candidates, function(y){
    if(length(which(dat.true[,1] == x[1] & dat.true[,2] == y) != 0)) {
      return(NULL)
    }
    return(y)
  }))
  return(data.frame(src=x[1], 
                    dst=sample(candidates, FALSE_PER_TRUE),
                    label=F))
}))

colnames(dat.true) <- c("src","dst","label")
dat <- rbind(dat.true,dat.false)
dat <- unique(dat)

## Adamic Adar

experiment.aa <- eval.aa(dat, DISCARD_REL)
write.csv(experiment.aa$raw, paste("../result/", OUTPUT_PREFIX ,".aa.csv",sep=""), row.names=F)

## Semantic Proximity

experiment.sp <- eval.sp(dat, DISCARD_REL)
write.csv(experiment.sp$raw, paste("../result/", OUTPUT_PREFIX ,".sp.csv",sep=""), row.names=F)

## Personalized PageRank

experiment.ppr <- eval.ppr(dat, DISCARD_REL)
write.csv(experiment.ppr$raw, paste("../result/", OUTPUT_PREFIX ,".ppr.csv",sep=""), row.names=F)

## Preferential Attachment

experiment.pa <- eval.pa(dat, DISCARD_REL)
write.csv(experiment.pa$raw, paste("../result/", OUTPUT_PREFIX ,".pa.csv",sep=""), row.names=F)

## Katz

experiment.katz <- eval.katz(dat, DISCARD_REL)
write.csv(experiment.katz$raw, paste("../result/", OUTPUT_PREFIX ,".katz.csv",sep=""), row.names=F)

## AMIE

experiment.amie <- eval.amie(dat, ASSOCIATE_REL)
write.csv(experiment.amie$raw, paste("../result/", OUTPUT_PREFIX ,".amie.csv",sep=""), row.names=F)

## Test Method

experiment.test <- eval.test(dat, DISCARD_REL)
write.csv(experiment.test$raw, paste("../result/", OUTPUT_PREFIX ,".test.csv",sep=""), row.names=F)

stopCluster(cl)