context("svg interactive functions")
library(xml2)

test_that("tracer is working", {
file <- tempfile(fileext = ".svg")
  dsvg( file = file, standalone = FALSE )
  plot.new()
  rvg_tracer_on()
  points(c(0.5, .6), c(.4, .3))
  ids = rvg_tracer_off()
  dev.off()

  expect_equal(length(ids), 2 )
  expect_equal(ids, 1:2 )

  doc <- read_xml(file)
  circle_id <- sapply( xml_find_all(doc, ".//circle"), xml_attr, "id" )
  expect_equal(circle_id, as.character(1:2) )
})

