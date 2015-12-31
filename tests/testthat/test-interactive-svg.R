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

test_that("tooltips are written", {
  file <- tempfile(fileext = ".svg")
  dsvg( file = file, standalone = FALSE, canvas_id = 1 )
  plot.new()
  rvg_tracer_on()
  points(c(0.5, .6), c(.4, .3))
  ids = rvg_tracer_off()
  send_tooltip(ids = ids, tooltips = c("tip1", "tip2") )
  dev.off()

  doc <- read_xml(file)
  script_node <- xml_find_one(doc, ".//script")
  script_txt <-  xml_text(script_node)
  tip1 <- "attr\\(\\'title\\',\\'tip1\\'\\)"
  tip2 <- "attr\\(\\'title\\',\\'tip2\\'\\)"
  expect_true( grepl( tip1, script_txt ) )
  expect_true( grepl( tip2, script_txt ) )
})

test_that("ID are written", {
  file <- tempfile(fileext = ".svg")
  dsvg( file = file, standalone = FALSE, canvas_id = 1 )
  plot.new()
  rvg_tracer_on()
  points(c(0.5, .6), c(.4, .3))
  ids = rvg_tracer_off()
  set_data_id(ids = ids, data_id = c("HI1", "HI2") )
  dev.off()

  doc <- read_xml(file)
  script_node <- xml_find_one(doc, ".//script")
  script_txt <-  xml_text(script_node)
  tip1 <- "attr\\(\\'data-id\\',\\'HI1\\'\\)"
  tip2 <- "attr\\(\\'data-id\\',\\'HI2\\'\\)"
  expect_true( grepl( tip1, script_txt ) )
  expect_true( grepl( tip2, script_txt ) )
})


test_that("clicks are written", {
  file <- tempfile(fileext = ".svg")
  dsvg( file = file, standalone = FALSE, canvas_id = 1 )
  plot.new()
  rvg_tracer_on()
  points(c(0.5, .6), c(.4, .3))
  ids = rvg_tracer_off()
  send_click(ids = ids, clicks = c("alert(1)", "alert(2)") )
  dev.off()

  doc <- read_xml(file)
  script_node <- xml_find_one(doc, ".//script")
  script_txt <-  xml_text(script_node)
  tip1 <- "click\\(alert\\(1\\)\\)"
  tip2 <- "click\\(alert\\(2\\)\\)"
  expect_true( grepl( tip1, script_txt ) )
  expect_true( grepl( tip2, script_txt ) )
})

