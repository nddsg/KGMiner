library(FSelector)
library(ggplot2)
library(bear)
library(RWeka)
source("experimentAPI.R")

fm_val <- function(resdf) {
  return(as.numeric(str_split(str_split(resdf$eval$string,"\n")[[1]][26], " +")[[1]][9]))
}

best_val <- function(dat, weights) {
  cl <- makeCluster(50) 
  clusterExport(cl = cl, varlist=c("dat", "weights", "eval.df", "cutoff.k", "fm_val","str_split",
                                   "as.numeric"), envir = environment())
  res <- rbind.fill(parLapply(cl, seq(1,ncol(dat)-1,by = 1), function(x){
    library(FSelector)
    library(RWeka)
    wekares<-eval.df(dat[,c("label", cutoff.k(weights, x))])
    return(data.frame(f1=fm_val(wekares),nfeature=x))
  }))
  return(res[which.max(res$f1),])
  stopCluster(cl)
}

feature_selection_validation <- function(filepath, label) {
  dat <- read.csv(filepath[1])
  weights <- information.gain(label~., dat)
  weights <- weights[order(weights[,1], decreasing = T),,drop=F]
  
  res <- NULL
  
  mpath_res <- eval.df(dat)
  
  res <- rbind(res,data.frame(test=label, type="Meta Path", nfeature = ncol(dat) - 1, roc=fm_val(mpath_res)))
  print(res)
  mpath_res <- best_val(dat, weights)
  
  res <- rbind(res,data.frame(test=label, type="Meta Path Subset", nfeature = mpath_res$nfeature, roc=mpath_res$f1))
  print(res)
  dat <- read.csv(filepath[2])
  weights <- information.gain(label~., dat)
  weights <- weights[order(weights[,1], decreasing = T),,drop=F]
  
  ppath_res <- eval.df(dat)
  
  res <- rbind(res,data.frame(test=label, type="Predicate Path", nfeature = ncol(dat) - 1, roc=fm_val(ppath_res)))
  print(res)
  mpath_res <- best_val(dat, weights)
  
  res <- rbind(res,data.frame(test=label, type="Predicate Path Subset", nfeature = mpath_res$nfeature, roc=mpath_res$f1))
  print(res)
  return(res)
  
}


res <- rbind(feature_selection_validation("../result/city/city_capital_all.test.csv", "city_capital"),
             feature_selection_validation("../result/city/capital_state_all.test.csv", "capital_state"),
             feature_selection_validation("../result/civil_war/battle_commander.test.csv", "civil war"),
             feature_selection_validation("../result/company/president_company_no_keyperson_all.test.csv", "company"),
             feature_selection_validation("../result/president/president.test.csv", "president-vice"),
             feature_selection_validation("../result/best_seller//best_seller.test.csv","best-seller"))

res.summary <- summarySE(res, measurevar = "value", groupvars = c("type"))
res.score.summary <- summarySE(res, measurevar = "score", groupvars = c("type"))
ggplot(res.summary, aes(x=type,y=value)) + geom_point() + geom_line() + geom_errorbar(aes(ymax=value+ci, ymin=value-ci), width=.3)