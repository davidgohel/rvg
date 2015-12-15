Sys.setenv("R_TESTS" = "")

library(testthat)
library(rvg)
test_check("rvg")
