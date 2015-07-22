library(utils)
library(stringr)

idpath2title <- function(li, mapfile) {
  return(unlist(lapply(as.character(li), function(x){
    paste(unlist(lapply(as.numeric(str_split(substr(x, 0, nchar(x)-1), ",")[[1]]), function(x){
      return(paste(ifelse(x < 0, "(-1)", ""), mapfile[which(mapfile$V1 == abs(x)), "V2"], sep=""))
    })), collapse=",")
  })))
}

hpath_parser <- function(path) {
  splitted <- str_split(substr(path, 1, nchar(path)-2),pattern = "--")[[1]]
  nodes <- splitted[seq(1,length(splitted),2)]
  edges <- str_replace_all(splitted[seq(2,length(splitted),2)], "[)(]","")
  return(list(nodes=nodes, edges=edges))
}

request <- function(command) {
  return(system(paste("echo \"", command, "\" | socat -t 3600 - UNIX-CONNECT:/tmp/gbserver", sep=""), intern = T))
}

heter_path <- function(id1, id2, discard_rel, max_depth = 3) {
  command <- paste("hpath", id1, id2, discard_rel, max_depth, "F", "F", sep=" ")
  
  paths <- request(command)
  return(lapply(unlist(str_split(paths[-1], "\n")), hpath_parser))
}

get_path_by_rel <- function(id1, id2, discard_rel, max_depth = 3, target_path=NULL) {
  tmp_res <- heter_path(id1, id2, discard_rel, max_depth)
  if(length(tmp_res) == 0) return(NULL)
  if(is.null(target_path)) {
    return(lapply(tmp_res, function(x){x[["nodes"]]}))
  }
  tmp_res <- lapply(tmp_res, function(x){
    if(sum(x[["edges"]] == target_path) == length(target_path)) {
      return(unlist(x[["nodes"]]))
    }
  })
  return(tmp_res[!sapply(tmp_res, is.null)])
}

rel_path <- function(id1, id2, max_depth = 3, is_directed = F, discard_rel, mapfile = NA, .raw=F) {
  library(utils)
  library(stringr)
  
  command <- paste("hpath", id1, id2, discard_rel, max_depth, ifelse(is_directed, "T", "F"), "P", sep=" ")
  
  paths <- request(command)
  npath <- as.numeric(str_split(paths[1], " ")[[1]][2])
  paths <- paths[-1]
  
  if(npath != length(paths)){
    print(paste("missing paths, expected",npath,"actual",length(paths)))
  }
  stopifnot(npath == length(paths))
  
  if (npath == 0) return(NULL)
  
  paths <- lapply(paths, function(x){
    edgetypes <- abs(as.numeric(str_split(substr(x,1,nchar(x)-1), ",")[[1]]))
    if(length(intersect(edgetypes, discard_rel)) == 0){
      return(x)
    } else {
      return(NULL)
    }
  })
  
  paths <- unlist(paths[!sapply(paths, is.null)])
    
  if (!.raw) {
    paths <- as.data.frame(table(paths))
    if (!is.na(mapfile) && is.data.frame(mapfile)) {
      paths$paths <- unlist(lapply(as.character(paths$paths), function(x){
        paste(unlist(lapply(as.numeric(str_split(substr(x, 0, nchar(x)-1), ",")[[1]]), function(x){
          return(paste(ifelse(x < 0, "(-1)", "-"), mapfile[which(mapfile$V1 == abs(x)), "V2"], sep=""))
        })), collapse=",")
      }))
    } else {
      paths$paths <- unlist(lapply(as.character(paths$paths), function(x){str_replace_all(x,"-","")}))
    }
    
    return(paths[order(-paths$Freq),])
    
  } else {
    
    if (!is.na(mapfile) && is.data.frame(mapfile)) {
      paths <- unlist(lapply(as.character(paths), function(x){
        paste(unlist(lapply(as.numeric(str_split(substr(x, 0, nchar(x)-1), ",")[[1]]), function(x){
          return(paste(ifelse(x < 0, "(-1)", "-"), mapfile[which(mapfile$V1 == abs(x)), "V2"], sep=""))
        })), collapse=",")
      }))
    } else {
      paths$paths <- unlist(lapply(as.character(paths$paths), function(x){str_replace_all(x,"-","")}))
    }
    return(paths)
  }  
}

get_neighbor_with_rel <- function(id, rel_type) {
  command <- paste("neighborwithrel", id, rel_type, sep=" ")
  tmpres <- str_split(request(command), ",")[[1]]
  return(as.numeric(tmpres[1:length(tmpres)-1]))
}

adamic_adar <- function(id1, id2, discard_rel) {
  command <- paste("aa", id1, id2, discard_rel, sep=" ")
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

semantic_proximity <- function(id1, id2, discard_rel) {
  command <- paste("sp", id1, id2, discard_rel, sep=" ")
  return(as.numeric(request(command)))
}

ppagerank <- function(src, dst, discard_rel) {
  command <- paste("ppr", src, dst, discard_rel, sep=" ")
  return(as.numeric(request(command)))
}

preferential_attachment <- function(id1, id2, discard_rel) {
  command <- paste("pa", id1, id2, discard_rel, sep=" ")
  return(as.numeric(request(command)))
}

katz <- function(id1, id2, discard_rel) {
  command <- paste("katz", id1, id2, discard_rel, sep=" ")
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