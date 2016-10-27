context("PPTX text")
library(xml2)
library(gdtools)

test_that("text can be found", {
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  text(0.2, 0.2, "hello")
  dev.off()

  doc <- read_xml(file)
  text_node <- xml_find_first(doc, ".//p:sp/p:txBody/a:p/a:r/a:t", ns = xml_ns( doc ))
  expect_is(object = text_node, class = "xml_node")
  expect_equal(xml_text(text_node), "hello")
})

test_that("cex affects strwidth", {
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  w1 <- strwidth("X")
  par(cex = 4)
  w4 <- strwidth("X")
  dev.off()
  expect_equal(w4 / w1, 4, tol = 1e-4)
})

test_that("special characters are escaped", {
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, 0.5, "<&>")
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  expect_equal(xml_text(xml_find_first(x, ".//p:sp/p:txBody/a:p/a:r/a:t", ns = ns )), "<&>")
})

test_that("utf-8 characters are preserved", {
  skip_on_os("windows") # skip because of xml2 buglet

  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, 0.5, "\u00b5")
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  expect_equal(xml_text(xml_find_first(x, ".//p:sp/p:txBody/a:p/a:r/a:t", ns = ns )), "\u00b5")
})

test_that("text color is written in fill attr", {
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, 0.5, "a", col = "#113399")
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  expect_equal( xml_attr( xml_find_first(x, ".//p:sp/p:txBody/a:p/a:r/a:rPr/a:solidFill/a:srgbClr", ns = ns ), "val" ), "113399" )
})

test_that("default point size is 12", {
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, 0.5, "a")
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  rPr <- xml_find_first(x, ".//p:sp/p:txBody/a:p/a:r/a:rPr", ns = ns )
  expect_equal(xml_attr(rPr, "sz"), "1200")
})

test_that("cex does not generate fractional font sizes", {
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, 0.5, "a", cex = .1)
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  rPr <- xml_find_first(x, ".//p:sp/p:txBody/a:p/a:r/a:rPr", ns = ns )
  expect_equal(xml_attr(rPr, "sz"), "120")
})

test_that("font sets weight/style", {
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, seq(0.9, 0.1, length = 4), "a", font = 1:4)
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  rPr <- xml_find_all(x, ".//p:sp/p:txBody/a:p/a:r/a:rPr", ns = ns )
  expect_equal(xml_attr(rPr, "b"), c(NA, "1", NA, "1"))
  expect_equal(xml_attr(rPr, "i"), c(NA, NA, "1", "1"))
})



test_that("font sets weight/style", {
  skip_if_not(font_family_exists("Arial"))
  skip_if_not(font_family_exists("Times New Roman"))
  skip_if_not(font_family_exists("Courier New"))

  file <- tempfile()
  dml_pptx( file = file, bg = "transparent",
            fonts = list(sans="Arial", serif = "Times New Roman",
                                mono = "Courier New"))
  plot.new()
  text(0.5, 0.1, "a", family = "serif")
  text(0.5, 0.5, "a", family = "sans")
  text(0.5, 0.9, "a", family = "mono")
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  rPr_latin <- xml_find_all(x, ".//p:sp/p:txBody/a:p/a:r/a:rPr/a:latin", ns = ns )
  rPr_cs <- xml_find_all(x, ".//p:sp/p:txBody/a:p/a:r/a:rPr/a:cs", ns = ns )
  expect_equal(xml_attr(rPr_latin, "typeface"), c("Times New Roman", "Arial", "Courier New"))
  expect_equal(xml_attr(rPr_cs, "typeface"), c("Times New Roman", "Arial", "Courier New"))
})

test_that("a symbol has width greater than 0", {
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent")
  plot(c(0,2), c(0,2), type = "n")
  strw <- strwidth(expression(symbol("\042")))
  dev.off()

  expect_gt(strw, 0)
})

test_that("symbol font family", {

  symbol_font <- gdtools::match_family("Symbol")
  file <- tempfile()
  dml_pptx( file = file, bg = "transparent",
            fonts = list(symbol = "Symbol"))
  plot(c(0,2), c(0,2), type = "n", axes = FALSE, xlab = "", ylab = "")
  text(1, 1, expression(symbol("\042")))
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  rPr_latin <- xml_find_first(x, ".//p:sp/p:txBody/a:p/a:r/a:rPr/a:latin", ns = ns )
  rPr_cs <- xml_find_first(x, ".//p:sp/p:txBody/a:p/a:r/a:rPr/a:cs", ns = ns )
  expect_equal(xml_attr(rPr_latin, "typeface"), symbol_font)
  expect_equal(xml_attr(rPr_cs, "typeface"), symbol_font)
})
