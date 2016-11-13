# ---- Cleanup everything before start ----
rm(list = ls())
gc()

### Find capital among most populous cities

# ---- GBSERVER API ----
source("./experimentAPI.R")

# ---- INPUT and CONFIGURATIONS ----

EDGE_TYPE_FILE = "../data/infobox.edgetypes" # Example : "../data/lobbyist.edgetypes"
INPUT_FILE = "../facts/city_capital.csv" # Example : "../facts/lobbyist/firm_payee.csv" col 1 and 2 are ids and 3 is label
CLUSTER_SIZE = 4 # Number of workers in gbserver
FALSE_PER_TRUE = 0
DISCARD_REL = 191
ASSOCIATE_REL = c(404)

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
dat <- read.csv(INPUT_FILE)

elapsed.time <- data.frame()

## Test Method

experiment.fullpath.test <- eval.fullpath.test(dat, DISCARD_REL)
write.csv(experiment.fullpath.test$raw, "../result/city/city_capital_all.fullpath.test.csv", row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="fullpath.test", 
                                               elapsed = experiment.fullpath.test$elapsed[3] * CLUSTER_SIZE / nrow(dat)))


print("FULL PATH TEST")
print(experiment.fullpath.test$eval)


experiment.test <- eval.test(dat, DISCARD_REL)
write.csv(experiment.test$raw, "../result/city/city_capital_all.test.csv", row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="test", 
                                               elapsed = experiment.test$elapsed[3] * CLUSTER_SIZE / nrow(dat)));

print("PREDICATE TEST")
print(experiment.test$eval)

q()

## Adamic Adar

experiment.aa <- eval.aa(dat, DISCARD_REL)
write.csv(experiment.aa$raw, "../result/city/city_capital_all.aa.csv", row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="aa", 
                                               elapsed = experiment.aa$elapsed[3] * CLUSTER_SIZE / nrow(dat)));

print("Adamic Adar")
print(experiment.aa$eval)

## Semantic Proximity

experiment.sp <- eval.sp(dat, DISCARD_REL)
write.csv(experiment.sp$raw, "../result/city/city_capital_all.sp.csv", row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="sp", 
                                               elapsed = experiment.sp$elapsed[3] * CLUSTER_SIZE / nrow(dat)));

print("Semantic Proximity")
print(experiment.sp$eval)


## Personalized PageRank

experiment.ppr <- eval.ppr(dat, DISCARD_REL)
write.csv(experiment.ppr$raw, "../result/city/city_capital_all.ppr.csv", row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="ppr", 
                                               elapsed = experiment.ppr$elapsed[3] * CLUSTER_SIZE / nrow(dat)));

print("Personalized PageRank")
print(experiment.ppr$eval)

## Preferential Attachment

experiment.pa <- eval.pa(dat, DISCARD_REL)
write.csv(experiment.pa$raw, "../result/city/city_capital_all.pa.csv", row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="pa", 
                                               elapsed = experiment.pa$elapsed[3] * CLUSTER_SIZE / nrow(dat)));

print("Preferential Attachment")
print(experiment.pa$eval)

## Katz

experiment.katz <- eval.katz(dat, DISCARD_REL)
write.csv(experiment.katz$raw, "../result/city/city_capital_all.katz.csv", row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="katz", 
                                               elapsed = experiment.katz$elapsed[3] * CLUSTER_SIZE / nrow(dat)));

print("Katz")
print(experiment.katz$eval)

## AMIE

experiment.amie <- eval.amie(dat, ASSOCIATE_REL)
write.csv(experiment.amie$raw, "../result/city/city_capital_all.amie.csv", row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="amie", 
                                               elapsed = experiment.amie$elapsed[3] * CLUSTER_SIZE / nrow(dat)));

print("AMIE")
print(experiment.amie$eval)






experiment.pcrwamie <- eval.pcrw(dat, c(404))
write.csv(experiment.pcrwamie$raw, "../result/city/city_capital_all.pcrwamie.csv", row.names=F)

elapsed.time <- rbind(elapsed.time, data.frame(method="pcrw", 
                                               elapsed = experiment.pcrwamie$elapsed[3] * CLUSTER_SIZE / nrow(dat)));

write.csv(elapsed.time, "../result/city/city_capital_all.elapsed.csv", row.names=F)

print("PCRW")
print(experiment.pcrwamie$eval)

stopCluster(cl)

## Simrank

#experiment.simrank <- read.csv("../facts/city_capital.simrank.csv", header=F)
#colnames(experiment.simrank) <- c("state_id", "city_id", "simrank")
#experiment.simrank <- merge(experiment.simrank, dat)[, c("isCapital","simrank")]
#colnames(experiment.simrank) <- c("label", "simrank")
#experiment.simrank <- eval.df(experiment.simrank)
#write.csv(experiment.simrank$raw, "../result/city/city_capital_all.simrank.csv", row.names=F)
