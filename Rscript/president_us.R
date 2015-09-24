# ---- Cleanup everything before start ----
rm(list = ls())
gc()

### Find battle commander pairs

# ---- GBSERVER API ----
source("./experimentAPI.R")

# ---- INPUT and CONFIGURATIONS ----

EDGE_TYPE_FILE = "../data/infobox.edgetypes" # Example : "../data/lobbyist.edgetypes"
INPUT_FILE = "../facts/government/president_vice_id.csv" # Example : "../facts/lobbyist/firm_payee.csv" col 1 and 2 are ids and 3 is label
CLUSTER_SIZE = 48 # Number of workers in gbserver
FALSE_PER_TRUE = 5
DISCARD_REL = 467
ASSOCIATE_REL = c(284,124,56)
OUTPUT_PREFIX = "president/president" # "result/" OUTPUT_PREFIX

# ---- Load edge type file ----

mapfile <- read.csv(EDGE_TYPE_FILE, sep="\t", header=F)
mapfile$V1 <- as.numeric(mapfile$V1)
mapfile$V2 <- as.character(mapfile$V2)

# ---- Init workers ----

cl <- makeCluster(CLUSTER_SIZE) 
clusterExport(cl = cl, varlist=c("adamic_adar", "semantic_proximity", "ppagerank", "heter_path",
                                 "preferential_attachment", "katz", "pcrw", "heter_full_path", "meta_path",
                                 "multidimensional_adamic_adar", "heterogeneous_adamic_adar",
                                 "connectedby", "rel_path", "truelabeled", "falselabeled", "str_split",
                                 "as.numeric", "request","DISCARD_REL"), envir = environment())


# ---- Load input data ----
dat.true <- unique(read.csv(INPUT_FILE))

if (ncol(dat.true) < 3)
  dat.true$label <- T

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
colnames(dat.false) <- c("src","dst","label")

dat <- rbind(dat.true,dat.false)
dat <- unique(dat)

elapsed.time <- data.frame()

## Adamic Adar

experiment.aa <- eval.aa(dat, DISCARD_REL)
write.csv(experiment.aa$raw, paste("../result/", OUTPUT_PREFIX ,".aa.csv",sep=""), row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="aa", 
                                               elapsed = experiment.aa$elapsed[3] * CLUSTER_SIZE / nrow(dat)));



## Semantic Proximity

experiment.sp <- eval.sp(dat, DISCARD_REL)
write.csv(experiment.sp$raw, paste("../result/", OUTPUT_PREFIX ,".sp.csv",sep=""), row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="sp", 
                                               elapsed = experiment.sp$elapsed[3] * CLUSTER_SIZE / nrow(dat)));



## Personalized PageRank

experiment.ppr <- eval.ppr(dat, DISCARD_REL)
write.csv(experiment.ppr$raw, paste("../result/", OUTPUT_PREFIX ,".ppr.csv",sep=""), row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="ppr", 
                                               elapsed = experiment.ppr$elapsed[3] * CLUSTER_SIZE / nrow(dat)));



## Preferential Attachment

experiment.pa <- eval.pa(dat, DISCARD_REL)
write.csv(experiment.pa$raw, paste("../result/", OUTPUT_PREFIX ,".pa.csv",sep=""), row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="pa", 
                                               elapsed = experiment.pa$elapsed[3] * CLUSTER_SIZE / nrow(dat)));



## Katz

experiment.katz <- eval.katz(dat, DISCARD_REL)
write.csv(experiment.katz$raw, paste("../result/", OUTPUT_PREFIX ,".katz.csv",sep=""), row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="katz", 
                                               elapsed = experiment.katz$elapsed[3] * CLUSTER_SIZE / nrow(dat)));



## AMIE

experiment.amie <- eval.amie(dat, ASSOCIATE_REL)
write.csv(experiment.amie$raw, paste("../result/", OUTPUT_PREFIX ,".amie.csv",sep=""), row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="amie", 
                                               elapsed = experiment.amie$elapsed[3] * CLUSTER_SIZE / nrow(dat)));




## Test Method

experiment.fullpath.test <- eval.fullpath.test(dat, DISCARD_REL)
write.csv(experiment.fullpath.test$raw, paste("../result/", OUTPUT_PREFIX ,".fullpath.test.csv",sep=""), row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="fullpath.test", 
                                               elapsed = experiment.fullpath.test$elapsed[3] * CLUSTER_SIZE / nrow(dat)))

experiment.test <- eval.test(dat, DISCARD_REL)
write.csv(experiment.test$raw, paste("../result/", OUTPUT_PREFIX ,".test.csv",sep=""), row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="test", 
                                               elapsed = experiment.test$elapsed[3] * CLUSTER_SIZE / nrow(dat)));




experiment.pcrwamie <- eval.pcrw(dat, c(284,124,56))
write.csv(experiment.pcrwamie$raw, paste("../result/", OUTPUT_PREFIX ,".pcrwamie.csv",sep=""), row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="pcrw", 
                                               elapsed = experiment.pcrwamie$elapsed[3] * CLUSTER_SIZE / nrow(dat)));

write.csv(elapsed.time, paste("../result/", OUTPUT_PREFIX ,".elapsed.csv",sep=""), row.names=F)


stopCluster(cl)

experiment.simrank <- read.csv("../facts/president.simrank.csv", header=F)
colnames(experiment.simrank) <- c("src", "dst", "score")
experiment.simrank <- merge(experiment.simrank, dat)[, c("label","score")]
experiment.simrank <- eval.df(experiment.simrank)
write.csv(experiment.simrank$raw, "../result/president//president.simrank.csv", row.names=F)
