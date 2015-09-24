# Test robustness of each algorithms with different data partition
library(FSelector)
library(ggplot2)
library(bear)
library(RWeka)
source("experimentAPI.R")

# Data frame format
# res.df <- data.frame(algorithm, partition, roc)

getRoc <- function(resdf) {
  return(as.numeric(str_split(str_split(resdf$eval$string,"\n")[[1]][26], " +")[[1]][9]))
}

best_val <- function(dat, weights, cl) {
  clusterExport(cl = cl, varlist=c("dat", "weights", "eval.df", "cutoff.k", "getRoc","str_split",
                                   "as.numeric"), envir = environment())
  res <- rbind.fill(parLapply(cl, seq(1,ncol(dat)-1,by = 1), function(x){
    library(FSelector)
    library(RWeka)
    wekares<-eval.df(dat[,c("label", cutoff.k(weights, x))])
    return(data.frame(roc=getRoc(wekares),nfeature=x))
  }))
  return(res[which.max(res$roc),])
}

calculate <- function(filepath, algorithm, fselection = F) {
  cl <- makeCluster(56) 
  
  set.seed(233)
  dat <- read.csv(filepath)
  
  ntrue <- length(which(dat$label == T))
  
  res <- NULL
  
  for(frac in seq(0.1,0.9,by=0.1)) {
    # Construct true labeled and false labeled data
    total <- floor(ntrue / frac)
    nfalse <- total - ntrue
    truelabeled <- dat[which(dat$label == T),]
    falselabeled <- dat[sample(which(dat$label == F), nfalse, replace = T),]
    
    ndat <- rbind(truelabeled, falselabeled)
    
    if (fselection) {
      weights <- information.gain(label~., ndat)
      rrr <- best_val(ndat, weights, cl)
      res <- rbind(res, data.frame(algorithm = algorithm, roc = rrr$roc, frac = frac, ntrue = ntrue, nfalse = nfalse))
    } else {
      res <- rbind(res, data.frame(algorithm = algorithm, roc = getRoc(eval.df(ndat)), frac = frac, ntrue = ntrue, nfalse = nfalse))
    }
    
  }
  
  stopCluster(cl)
  
  return(res)
}

savegraph <- function(df, filepath) {
  g <- ggplot(df, aes(x=frac, y=roc, shape=algorithm, color=algorithm)) + 
    geom_point(size=4, position = "dodge") + geom_line() +
    ylab("AUROC") +
    xlab("Precentage of True Labeled Data") +
    scale_shape_manual(values=c(16,0,1,2,5,6,3,4,18)) + 
    scale_color_manual(values=c("blue",rep("red",8))) +
    theme_classic() +
    theme(panel.background = element_rect(colour = "black", size=1),          
          legend.title=element_blank(),
          legend.position="none",
          legend.background = element_rect(fill = "transparent"))
  ggsave(filepath, g, width=5,height=5)
}

capital_state <- rbind(calculate("../result/city/capital_state_all.test.csv", "test_method", T),
                      calculate("../result/city/capital_state_all.amie.csv", "AMIE"),
                      calculate("../result/city/capital_state_all.katz.csv", "KATZ"),
                      calculate("../result/city/capital_state_all.pa.csv", "Preferential Attachment"),
                      calculate("../result/city/capital_state_all.pcrwamie.csv", "PCRW"),
                      calculate("../result/city/capital_state_all.ppr.csv", "PPR"),
                      calculate("../result/city/state_capital_all.simrank.csv", "SimRank"),
                      calculate("../result/city/capital_state_all.sp.csv", "Semantic Proximity"),
                      calculate("../result/city/capital_state_all.aa.csv", "Adamic Adar"))

savegraph(capital_state, "../result/robustness/capital_state.eps")

city_capital <- rbind(calculate("../result/city/city_capital_all.test.csv", "test_method", T),
                       calculate("../result/city/city_capital_all.amie.csv", "AMIE"),
                       calculate("../result/city/city_capital_all.katz.csv", "KATZ"),
                       calculate("../result/city/city_capital_all.pa.csv", "Preferential Attachment"),
                       calculate("../result/city/city_capital_all.pcrwamie.csv", "PCRW"),
                       calculate("../result/city/city_capital_all.ppr.csv", "PPR"),
                       calculate("../result/city//city_capital_all.simrank.csv", "SimRank"),
                       calculate("../result/city/city_capital_all.sp.csv", "Semantic Proximity"),
                      calculate("../result/city/city_capital_all.aa.csv", "Adamic Adar"))



savegraph(city_capital, "../result/robustness/city_capital.eps")

company_president <- rbind(calculate("../result/company/president_company_no_keyperson_all.test.csv", "test_method", T),
                      calculate("../result/company/president_company_no_keyperson_all.amie.csv", "AMIE"),
                      calculate("../result/company/president_company_no_keyperson_all.katz.csv", "KATZ"),
                      calculate("../result/company/president_company_no_keyperson_all.pa.csv", "Preferential Attachment"),
                      calculate("../result/company/president_company_no_keyperson_all.pcrwamie.csv", "PCRW"),
                      calculate("../result/company/president_company_no_keyperson_all.ppr.csv", "PPR"),
                      calculate("../result/company/president_company_no_keyperson_all.simrank.csv", "SimRank"),
                      calculate("../result/company/president_company_no_keyperson_all.sp.csv", "Semantic Proximity"),
                      calculate("../result/company/president_company_no_keyperson_all.aa.csv", "Adamic Adar"))


savegraph(company_president, "../result/robustness/company_president.eps")


nyt_bestseller <- rbind(calculate("../result/best_seller/best_seller.test.csv", "test_method", T),
                        calculate("../result/best_seller/best_seller.amie.csv", "AMIE"),
                        calculate("../result/best_seller/best_seller.katz.csv", "KATZ"),
                        calculate("../result/best_seller/best_seller.pa.csv", "Preferential Attachment"),
                        calculate("../result/best_seller/best_seller.pcrwamie.csv", "PCRW"),
                        calculate("../result/best_seller/best_seller.ppr.csv", "PPR"),
                        calculate("../result/best_seller/best_seller.simrank.csv", "SimRank"),
                        calculate("../result/best_seller/best_seller.sp.csv", "Semantic Proximity"),
                        calculate("../result/best_seller/best_seller.aa.csv", "Adamic Adar"))
                        
                        

savegraph(nyt_bestseller, "../result/robustness/nyt_bestseller.eps")


civil_war <- rbind(calculate("../result/civil_war/battle_commander.test.csv", "test_method", T),
                   calculate("../result/civil_war/battle_commander.amie.csv", "AMIE"),
                   calculate("../result/civil_war/battle_commander.katz.csv", "KATZ"),
                   calculate("../result/civil_war/battle_commander.pa.csv", "Preferential Attachment"),
                   calculate("../result/civil_war/battle_commander.pcrwamie.csv", "PCRW"),
                   calculate("../result/civil_war/battle_commander.ppr.csv", "PPR"),
                   calculate("../result/civil_war/civil_war.simrank.csv", "SimRank"),
                   calculate("../result/civil_war/battle_commander.sp.csv", "Semantic Proximity"),
                   calculate("../result/civil_war/battle_commander.aa.csv", "Adamic Adar"))



savegraph(civil_war, "../result/robustness/civil_war.eps")

uspresident <- rbind(calculate("../result/president/president.test.csv", "test_method", T),
                     calculate("../result/president/president.amie.csv", "AMIE"),
                     calculate("../result/president/president.katz.csv", "KATZ"),
                     calculate("../result/president/president.pa.csv", "Preferential Attachment"),
                     calculate("../result/president/president.pcrwamie.csv", "PCRW"),
                     calculate("../result/president/president.ppr.csv", "PPR"),
                     calculate("../result/president/president.simrank.csv", "SimRank"),
                     calculate("../result/president/president.sp.csv", "Semantic Proximity"),
                     calculate("../result/president/president.aa.csv", "Adamic Adar"))


savegraph(uspresident, "../result/robustness/uspresident.eps")

aapp_dsyn <- rbind(calculate("../result/aapp_dsyn/aapp_dsyn.test.csv", "test_method", T),
                   calculate("../result/aapp_dsyn/aapp_dsyn.amie.csv", "AMIE"),
                   calculate("../result/aapp_dsyn/aapp_dsyn.katz.csv", "KATZ"),
                   calculate("../result/aapp_dsyn/aapp_dsyn.pa.csv", "Preferential Attachment"),
                   calculate("../result/aapp_dsyn/aapp_dsyn.pcrwamie.csv", "PCRW"),
                   calculate("../result/aapp_dsyn/aapp_dsyn.ppr.csv", "PPR"),
                   calculate("../result/aapp_dsyn/aapp_dsyn.simrank.csv", "SimRank"),
                   calculate("../result/aapp_dsyn/aapp_dsyn.sp.csv", "Semantic Proximity"),
                   calculate("../result/aapp_dsyn/aapp_dsyn.aa.csv", "Adamic Adar"))
savegraph(aapp_dsyn, "../result/robustness/aapp_dsyn.eps")

gngm_celf <- rbind(calculate("../result/gngm_celf/gngm_celf.test.csv", "test_method", T),
                   calculate("../result/gngm_celf/gngm_celf.amie.csv", "AMIE"),
                   calculate("../result/gngm_celf/gngm_celf.katz.csv", "KATZ"),
                   calculate("../result/gngm_celf/gngm_celf.pa.csv", "Preferential Attachment"),
                   calculate("../result/gngm_celf/gngm_celf.pcrwamie.csv", "PCRW"),
                   calculate("../result/gngm_celf/gngm_celf.ppr.csv", "PPR"),
                   calculate("../result/gngm_celf/gngm_celf.simrank.csv", "SimRank"),
                   calculate("../result/gngm_celf/gngm_celf.sp.csv", "Semantic Proximity"),
                   calculate("../result/gngm_celf/gngm_celf.aa.csv", "Adamic Adar"))
savegraph(gngm_celf, "../result/robustness/gngm_celf.eps")