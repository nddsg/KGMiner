library(utils)
library(plyr)
library(LiblineaR)
library(parallel)
library(RWeka)

source("./gbserverAPI.R")

## All input dfs must have at least three columns, first two columns are ids and the third column is class label.

eval.df <- function(df) {
  res <- list()
  res[["raw"]] <- df
  res[["model"]] <- Logistic(label~.,res[["raw"]])
  res[["eval"]] <- evaluate_Weka_classifier(res[["model"]], numFolds = 10, complexity = T, class = T, seed = 233)
  return(res)
}

## Basic experiment helper
eval.helper <- function(df, discard_rel, gen_func) {
  res <- list()
  res[["raw"]] <- gen_func(df, discard_rel)
  res[["model"]] <- Logistic(label~.,res[["raw"]])
  res[["eval"]] <- evaluate_Weka_classifier(res[["model"]], numFolds = 10, complexity = T, class = T, seed = 233)
  return(res)
}

## Adamic Adar
eval.aa <- function(df, discard_rel) {
  func.aa <- function(df, discard_rel) {
    return(data.frame(label=df[,3], aa = parApply(cl, df, 1, function(x){
      return(adamic_adar(as.numeric(x[1]), as.numeric(x[2]), discard_rel))
    })))
  }
  return(eval.helper(df, discard_rel, func.aa))
}

## Semantic Proximity
eval.sp <- function(df, discard_rel) {
  func.sp <- function(df, discard_rel) {
    return(data.frame(label = df[,3], sp = parApply(cl, df, 1, function(x){
      return(semantic_proximity(as.numeric(x[1]), as.numeric(x[2]), discard_rel))
    })))
  }
  return(eval.helper(df, discard_rel, func.sp))
}

## Personalized PageRank
eval.ppr <- function(df, discard_rel) {
  func.ppr <- function(df, discard_rel) {
    return(data.frame(label = df[,3], ppr = parApply(cl, df, 1, function(x){
      return(ppagerank(as.numeric(x[1]), as.numeric(x[2]), discard_rel))
    })))
  }
  return(eval.helper(df, discard_rel, func.ppr))
}

## Path Constrainted Random Walk
eval.pcrw <- function(df, metapaths) {
  func.pcrw <- function(df, metapaths) {
    tmp.df <- as.data.frame(df[,3])
    
    for(metapath in metapaths) {
      tmp.df <- cbind(tmp.df, parApply(cl, df, 1, function(y){
        return(pcrw(as.numeric(y[1]), as.numeric(y[2]), metapath))
      }))
    }
    colnames(tmp.df) <- c("label",paste("feature", 1:(ncol(tmp.df)-1), sep = "_"))
    return(tmp.df)
  }
  return(eval.helper(df, metapaths, func.pcrw))
}

## Preferential Attachment
eval.pa <- function(df, discard_rel) {
  func.pa <- function(df, discard_rel) {
    return(data.frame(label = df[,3], pa = parApply(cl, df, 1, function(x){
      return(preferential_attachment(as.numeric(x[1]), as.numeric(x[2]), discard_rel))
    })))
  }
  return(eval.helper(df, discard_rel, func.pa))
}

## Katz
eval.katz <- function(df, discard_rel) {
  func.katz <- function(df, discard_rel) {
    return(data.frame(label = df[,3], katz = parApply(cl, df, 1, function(x){
      return(katz(as.numeric(x[1]), as.numeric(x[2]), discard_rel))
    })))
  }
  return(eval.helper(df, discard_rel, func.katz))
}

## AMIE
eval.amie <- function(df, associated_rel) {
  func.amie <- function(df, associated_rel) {
    tmp.df <- as.data.frame(df[,3])
    
    for(rule in associated_rel) {
      if (length(rule) == 1) {
        tmp.df <- cbind(tmp.df, parApply(cl, df, 1, function(y){
          return(connectedby(as.numeric(y[1]), as.numeric(y[2]), rel_type = as.numeric(rule[1])))}))
      }
    }
    colnames(tmp.df) <- c("label",paste("feature", 1:(ncol(tmp.df)-1), sep = "_"))
    return(tmp.df)
  }
  return(eval.helper(df, associated_rel, func.amie))
}

## Test method
eval.test <- function(df, discard_rel) {
  func.test <- function(df, discard_rel) {
    tmp.paths <- rbind.fill(parApply(cl, dat, 1, function(x) {
      tmp_paths <- rel_path(as.numeric(x[1]), as.numeric(x[2]), 3,F, DISCARD_REL)
      if(length(tmp_paths) == 0) {
        return(data.frame(label = as.logical(x[3])))
      }
      rtn <- as.data.frame(t(tmp_paths$Freq))
      colnames(rtn) <- tmp_paths$paths
      rtn <- cbind(label = as.logical(x[3]), rtn)
      return(rtn)
    }))
    tmp.paths[is.na(tmp.paths)] <- 0
    return(tmp.paths)
  }
  return(eval.helper(df, discard_rel, func.test))
}