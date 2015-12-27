context("PPTX Lines")
library(xml2)

test_that("segments don't have fill", {

  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  segments(0.5, 0.5, 1, 1)
  dev.off()

  doc <- read_xml(file)
  fill_node <- try(xml_find_one(doc, ".//p:sp/p:spPr/a:solidFill", ns = xml_ns( doc ) ), silent = TRUE )
  expect_true( inherits(fill_node, "try-error") )
})

test_that("lines don't have fill", {

  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  lines(c(0.5, 1, 0.5), c(0.5, 1, 1))
  dev.off()

  doc <- read_xml(file)
  fill_node <- try(xml_find_one(doc, ".//p:sp/p:spPr/a:solidFill", ns = xml_ns( doc ) ), silent = TRUE )
  expect_true( inherits(fill_node, "try-error") )
})

test_that("polygons do have fill", {
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  polygon(c(0.5, 1, 0.5), c(0.5, 1, 1), col = "red", border = "blue")
  dev.off()

  doc <- read_xml(file)
  fill_node <- try(xml_find_one(doc, ".//p:sp/p:spPr/a:solidFill", ns = xml_ns( doc ) ), silent = TRUE )
  expect_false( inherits(fill_node, "try-error") )
})

test_that("polygons without border", {
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  polygon(c(0.5, 1, 0.5), c(0.5, 1, 1), col = "red", border = NA)
  dev.off()

  doc <- read_xml(file)
  fill_color <- try(xml_find_one(doc, ".//p:sp/p:spPr/a:solidFill/a:srgbClr", ns = xml_ns( doc ) ), silent = TRUE )
  expect_equal(xml_attr(fill_color, "val"), "FF0000")

  line_color <- try( xml_find_one(doc, ".//p:sp/p:spPr/a:ln", ns = xml_ns( doc )), silent = TRUE )
  expect_true( inherits(line_color, "try-error") )

})

mini_plot <- function(...) graphics::plot(..., axes = FALSE, xlab = "", ylab = "")

dash_array <- function(...) {
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  mini_plot(1:3, ..., type = "l")
  dev.off()

  doc <- read_xml(file)
  dash <- try(xml_find_one(doc, ".//p:sp/p:spPr/a:ln/a:prstDash", ns = xml_ns( doc )), silent = TRUE )
  dash
}
custom_dash_array <- function(...) {
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  mini_plot(1:3, ..., type = "l")
  dev.off()

  doc <- read_xml(file)
  dash <- xml_find_all(doc, ".//p:sp/p:spPr/a:ln/a:custDash/a:ds", ns = xml_ns( doc ))
  as.character( unlist(lapply( dash, xml_attrs)) )
}

test_that("lty are ok", {
  expect_equal(xml_attr(dash_array(lty = 1), "val"), "solid")
  expect_equal(xml_attr(dash_array(lty = 2), "val"), "dash")
  expect_equal(xml_attr(dash_array(lty = 3), "val"), "dot")
  expect_equal(custom_dash_array(lty = 4), c("100000", "300000", "400000", "300000"))
  expect_equal(xml_attr(dash_array(lty = 5), "val"), "lgDash")
  expect_equal(custom_dash_array(lty = 6), c("200000", "200000", "600000", "200000"))
  expect_equal(custom_dash_array(lty = "1F"), c("100000", "1500000"))
  expect_equal(custom_dash_array(lty = "1234"), c("100000", "200000", "300000", "400000"))
})

test_that("lty scales with lwd", {
  expect_equal(custom_dash_array(lty = 4), c("100000", "300000", "400000", "300000"))
  expect_equal(custom_dash_array(lty = 4, lwd = 2), c("200000", "600000", "800000", "600000"))
})


test_that("line join shapes", {
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  lines(c(0.3, 0.5, 0.7), c(0.1, 0.9, 0.1), lwd = 15, ljoin = "round")
  dev.off()

  doc <- read_xml(file)
  join_shape <- try(xml_find_one(doc, ".//p:sp/p:spPr/a:ln/a:round", ns = xml_ns( doc ) ), silent = TRUE )
  expect_false( inherits(join_shape, "try-error") )

  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  lines(c(0.3, 0.5, 0.7), c(0.1, 0.9, 0.1), lwd = 15, ljoin = "mitre")
  dev.off()

  doc <- read_xml(file)
  join_shape <- try(xml_find_one(doc, ".//p:sp/p:spPr/a:ln/a:miter", ns = xml_ns( doc ) ), silent = TRUE )
  expect_false( inherits(join_shape, "try-error") )

  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  lines(c(0.3, 0.5, 0.7), c(0.1, 0.9, 0.1), lwd = 15, ljoin = "bevel")
  dev.off()

  doc <- read_xml(file)
  join_shape <- try(xml_find_one(doc, ".//p:sp/p:spPr/a:ln/a:bevel", ns = xml_ns( doc ) ), silent = TRUE )
  expect_false( inherits(join_shape, "try-error") )
})
