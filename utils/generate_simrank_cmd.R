EDGELIST = "../infobox.edgelist"
INPUT_FILE = "../facts/city_capital.csv"
OUTPUT_PATH = "../simrank/city_capital/"
SIMRANK_PATH = "../wSimund"
SIMRANK_OUTPUT_PATH = "../results/city_capital/"
SIMRANK_PARAMS = "1 100 100000 0.8 20"

dir.create(paste(OUTPUT_PATH, "true", sep="/"), recursive = T)
dir.create(paste(OUTPUT_PATH, "false", sep="/"), recursive = T)

dat <- read.csv(INPUT_FILE)
dat.true <- dat[dat[,3],1:2]
dat.false <- dat[!dat[,3],1:2]

cmd.true <- apply(dat.true, 1, function(x){
  write(paste(x, collapse = " "), paste(OUTPUT_PATH, "/true/", paste(x, collapse = "_"), sep=""))
  return(paste(SIMRANK_PATH, EDGELIST, paste(SIMRANK_OUTPUT_PATH, "/true/",paste(x,collapse="_"),sep=""),
                                          SIMRANK_PARAMS, paste(OUTPUT_PATH, "/true/", paste(x, collapse = "_"), sep=""), sep = " "))
})

write(paste(cmd.true, collapse = "\n"), paste(OUTPUT_PATH, "/true.cmd", sep=""))

cmd.false <- lapply(unique(dat.false[,1]), function(x){
  dest <- unique(dat.false[which(dat.false[,1] == x),2])
  write(paste(c(x, dest), collapse = " "), paste(OUTPUT_PATH, "/false/", paste(c(x, dest), collapse = "_"), sep=""))
  return(paste(SIMRANK_PATH, EDGELIST, paste(SIMRANK_OUTPUT_PATH, "/false/",paste(c(x, dest), collapse = "_"),sep=""),
               SIMRANK_PARAMS, paste(OUTPUT_PATH, "/false/", paste(c(x, dest), collapse = "_"), sep=""), sep = " "))
})

write(paste(cmd.false, collapse = "\n"), paste(OUTPUT_PATH, "/false.cmd", sep=""))

