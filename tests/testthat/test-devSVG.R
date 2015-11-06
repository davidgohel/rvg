context("dev dsvg")
library(xml2)

test_that("adds default background", {
  x <- xmlDSVG(plot.new())
  expect_equal(xml_attr(xml_find_one(x, ".//rect"), "fill"), "#FFFFFF")
})

test_that("default background respects par", {
  x <- xmlDSVG({
    par(bg = "red")
    plot.new()
  })
  expect_equal(xml_attr(xml_find_one(x, ".//rect"), "fill"), rgb(1, 0, 0))
})

test_that("can only have one page", {
  dsvg(tempfile())
  on.exit(dev.off())

  plot.new()
  expect_error(plot.new(), "only supports one page")
})
