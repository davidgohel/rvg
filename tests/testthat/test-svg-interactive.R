context("dsvg interactive functions")
library(xml2)
library(grid)

# tracer -------
test_that("tracer is working", {
  file <- tempfile(fileext = ".svg")
  dsvg( file = file, standalone = FALSE, bg = "transparent" )
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

# send_tooltip -------
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

# set_data_id -------
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


# send_click -------
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

# set_attr -------
test_that("attributes are written", {
  file <- tempfile(fileext = ".svg")
  dsvg( file = file, standalone = FALSE, canvas_id = 1 )
  plot.new()
  rvg_tracer_on()
  points(c(0.5, .6), c(.4, .3))
  ids = rvg_tracer_off()
  set_attr(ids = ids, str = c("alert(1)", "alert(2)"), attribute = "onclick" )
  dev.off()

  doc <- read_xml(file)
  script_node <- xml_find_one(doc, ".//script")
  script_txt <-  xml_text(script_node)
  tip1 = "document\\.querySelectorAll\\('#svg_[0-9]+'\\)\\[0\\]\\.getElementById\\('[0-9]+'\\)\\.setAttribute\\('onclick','alert\\(1\\)'\\)"
  tip2 = "document\\.querySelectorAll\\('#svg_[0-9]+'\\)\\[0\\]\\.getElementById\\('[0-9]+'\\)\\.setAttribute\\('onclick','alert\\(2\\)'\\)"
  expect_true( grepl( tip1, script_txt ) )
  expect_true( grepl( tip2, script_txt ) )
})

test_that("attributes cannot contain single quotes", {
  file <- tempfile(fileext = ".svg")
  dsvg( file = file, standalone = FALSE, canvas_id = 1 )
  plot.new()
  rvg_tracer_on()
  points(c(0.5, .6), c(.4, .3))
  ids = rvg_tracer_off()
  expect_error(
    set_attr(ids = ids, str = c("alert('1')", "alert('2')"), attribute = "onclick" )
  )
  dev.off()
})



# interactive grobs -------

test_that("interactive grid point grobs", {
  file <- tempfile(fileext = ".svg")
  dsvg( file = file, standalone = FALSE, canvas_id = 1 )
  gl <- interactivePointsGrob(tooltip = "tooltip")
  gl <- editGrob(gl, gp = gpar(col = "green"))
  grid.draw(gl)
  dev.off()

  doc <- read_xml(file)
  script_node <- xml_find_one(doc, ".//script")
  script_txt <-  xml_text(script_node)
  tip <- "attr\\(\\'title\\',\\'tooltip\\'\\)"
  expect_true( grepl( tip, script_txt ) )
})

test_that("interactive grid polygon grobs", {
  file <- tempfile(fileext = ".svg")
  dsvg( file = file, standalone = FALSE, canvas_id = 1 )
  gl <- interactivePolygonGrob(x=c(0, 0.5, 1, 0.5), y=c(0.5, 1, 0.5, 0),
                               tooltip = "tooltip")
  gl <- editGrob(gl, gp = gpar(col = "green"))
  grid.draw(gl)
  dev.off()

  doc <- read_xml(file)
  script_node <- xml_find_one(doc, ".//script")
  script_txt <-  xml_text(script_node)
  tip <- "attr\\(\\'title\\',\\'tooltip\\'\\)"
  expect_true( grepl( tip, script_txt ) )
})


test_that("interactive grid polyline grobs", {
  file <- tempfile(fileext = ".svg")
  dsvg( file = file, standalone = FALSE, canvas_id = 1 )
  gl <- interactivePolylineGrob(x=c(0, 0.5, 1, 0.5), y=c(0.5, 1, 0.5, 0),
                               tooltip = "tooltip")
  gl <- editGrob(gl, gp = gpar(col = "blue"))
  grid.draw(gl)
  dev.off()

  doc <- read_xml(file)
  script_node <- xml_find_one(doc, ".//script")
  script_txt <-  xml_text(script_node)
  tip <- "attr\\(\\'title\\',\\'tooltip\\'\\)"
  expect_true( grepl( tip, script_txt ) )
})


test_that("interactive grid rect grobs", {
  file <- tempfile(fileext = ".svg")
  dsvg( file = file, standalone = FALSE, canvas_id = 1 )
  gl <- interactiveRectGrob(tooltip = "tooltip")
  gl <- editGrob(gl, gp = gpar(col = "blue"))
  grid.draw(gl)
  dev.off()

  doc <- read_xml(file)
  script_node <- xml_find_one(doc, ".//script")
  script_txt <-  xml_text(script_node)
  tip <- "attr\\(\\'title\\',\\'tooltip\\'\\)"
  expect_true( grepl( tip, script_txt ) )
})


test_that("interactive grid segment grobs", {
  file <- tempfile(fileext = ".svg")
  dsvg( file = file, standalone = FALSE, canvas_id = 1 )
  gl <- interactiveSegmentsGrob(tooltip = "tooltip")
  gl <- editGrob(gl)
  grid.draw(gl)
  dev.off()

  doc <- read_xml(file)
  script_node <- xml_find_one(doc, ".//script")
  script_txt <-  xml_text(script_node)
  tip <- "attr\\(\\'title\\',\\'tooltip\\'\\)"
  expect_true( grepl( tip, script_txt ) )
})

test_that("interactive grid text grobs", {
  file <- tempfile(fileext = ".svg")
  dsvg( file = file, standalone = FALSE, canvas_id = 1 )
  gl <- interactiveTextGrob(label = "text", tooltip = "tooltip")
  gl <- editGrob(gl)
  grid.draw(gl)
  dev.off()

  doc <- read_xml(file)
  script_node <- xml_find_one(doc, ".//script")
  script_txt <-  xml_text(script_node)
  tip <- "attr\\(\\'title\\',\\'tooltip\\'\\)"
  expect_true( grepl( tip, script_txt ) )
})

