library(utils)
library(stringr)

idpath2title <- function(li, mapfile) {
  return(unlist(lapply(as.character(li), function(x){
    paste(unlist(lapply(as.numeric(str_split(substr(x, 0, nchar(x)-1), ",")[[1]]), function(x){
      return(paste(ifelse(x < 0, "(-1)", ""), mapfile[which(mapfile$V1 == abs(x)), "V2"], sep=""))
    })), collapse=",")
  })))
}

request <- function(command) {
  return(system(paste("echo \"", command, "\" | socat -t 3600 - UNIX-CONNECT:/tmp/gbserver", sep=""), intern = T))
}

rel_path <- function(id1, id2, max_depth = 3, is_directed = F, discard_rel = 191, mapfile = NA, .raw=F) {
  library(utils)


  command <- paste("hpath", id1, id2, max_depth, ifelse(is_directed, "T", "F"), "P", sep=" ")

  request(command)
  paths <- request(command)
  npath <- as.numeric(str_split(paths[1], " ")[[1]][2])
  paths <- paths[-1]

  stopifnot(npath == length(paths))

  paths <- paths[which(!grepl(discard_rel, paths))]

  if (!.raw) {

    paths <- as.data.frame(table(paths))
    if (!is.na(mapfile) && is.data.frame(mapfile)) {
      paths$paths <- unlist(lapply(as.character(paths$paths), function(x){
        paste(unlist(lapply(as.numeric(str_split(substr(x, 0, nchar(x)-1), ",")[[1]]), function(x){
          return(paste(ifelse(x < 0, "(-1)", ""), mapfile[which(mapfile$V1 == abs(x)), "V2"], sep=""))
        })), collapse=",")
      }))
    }

    return(paths[order(-paths$Freq),])

  } else {

    if (!is.na(mapfile) && is.data.frame(mapfile)) {
      paths <- unlist(lapply(as.character(paths), function(x){
        paste(unlist(lapply(as.numeric(str_split(substr(x, 0, nchar(x)-1), ",")[[1]]), function(x){
          return(paste(ifelse(x < 0, "(-1)", ""), mapfile[which(mapfile$V1 == abs(x)), "V2"], sep=""))
        })), collapse=",")
      }))
    }
    return(paths)
  }
}

adamic_adar <- function(id1, id2) {
  command <- paste("aa", id1, id2, sep=" ")
  return(as.numeric(request(command)))
}

multidimensional_adamic_adar <- function(id1, id2, rel_type = 191) {
  command <- paste("maa", id1, id2, rel_type, sep=" ")
  return(as.numeric(request(command)))
}

heterogeneous_adamic_adar <- function(id1, id2, rel_type = 191) {
  command <- paste("haa", id1, id2, rel_type, sep=" ")
  return(as.numeric(request(command)))
}

semantic_proximity <- function(id1, id2) {
  command <- paste("sp", id1, id2, sep=" ")
  return(as.numeric(request(command)))
}

ppagerank <- function(src, dst) {
  command <- paste("ppr", src, dst, sep=" ")
  return(as.numeric(request(command)))
}

preferential_attachment <- function(id1, id2) {
  command <- paste("pa", id1, id2, sep=" ")
  return(as.numeric(request(command)))
}

katz <- function(id1, id2) {
  command <- paste("katz", id1, id2, sep=" ")
  return(as.numeric(request(command)))
}

connectedby <- function(src, dst, rel_type = 404) {
  command <- paste("connectedby", src, dst, rel_type, sep=" ")
  return(as.numeric(request(command)))
}

truelabeled <- function(src, dst, rel_type = 191, .semantic = F) {
  command <- paste("truelabeled", src, dst, rel_type, ifelse(.semantic, "T", "F"), sep=" ")
  restext <- textConnection(request(command))
  resdf <- read.csv(restext, header = F, sep = "\t")
  colnames(resdf) <- c("src","dst")
  close(restext)
  return(resdf)
}

falselabeled <- function(src, dst, rel_type = 191, .semantic = F) {
  command <- paste("falselabeled", src, dst, rel_type, ifelse(.semantic, "T", "F"), sep=" ")
  restext <- textConnection(request(command))
  resdf <- read.csv(restext, header = F, sep = "\t")
  colnames(resdf) <- c("src","dst")
  close(restext)
  return(resdf)
}