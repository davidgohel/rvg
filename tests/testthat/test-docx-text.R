context("DOCX text")
library(xml2)

test_that("text can be found", {

  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  text(0.2, 0.2, "hello")
  dev.off()

  x <- read_xml(file)
  xpath_ <- ".//wps:wsp/wps:txbx/w:txbxContent/w:p/w:r/w:t"
  text_node <- xml_find_first(x, xpath_, ns = xml_ns( x ))
  expect_is(object = text_node, class = "xml_node")
  expect_equal(xml_text(text_node), "hello")
})

test_that("text alpha col can be found", {

  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  text(0.2, 0.2, "hello", col = "#00000099")
  dev.off()

  x <- scan(file, what = "character", sep = "\n", quiet = TRUE)
  expect_true(grepl("<w14:alpha val=\"60000\"/>", x))
})


test_that("cex affects strwidth", {
  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  w1 <- strwidth("X")
  par(cex = 4)
  w4 <- strwidth("X")
  dev.off()
  expect_equal(w4 / w1, 4, tol = 1e-4)
})

test_that("special characters are escaped", {
  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, 0.5, "<&>")
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  xpath_ <- ".//wps:wsp/wps:txbx/w:txbxContent/w:p/w:r/w:t"
  text_node <- xml_find_first(x, xpath_, ns = xml_ns( x ))
  expect_equal(xml_text(text_node), "<&>")
})

test_that("utf-8 characters are preserved", {
  skip_on_os("windows") # skip because of xml2 buglet

  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, 0.5, "\u00b5")
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  xpath_ <- ".//wps:wsp/wps:txbx/w:txbxContent/w:p/w:r/w:t"
  text_node <- xml_find_first(x, xpath_, ns = xml_ns( x ))
  expect_equal(xml_text(text_node), "\u00b5")
})

test_that("text color is written in fill attr", {
  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, 0.5, "a", col = "#113399")
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  xpath_ <- ".//wps:wsp/wps:txbx/w:txbxContent/w:p/w:r/w:rPr/w:color"
  selected_node <- xml_find_first(x, xpath_, ns = xml_ns( x ))
  expect_equal(xml_attr(selected_node, "val"), "113399")
})

test_that("default point size is 12", {
  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, 0.5, "a")
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  xpath_ <- ".//wps:wsp/wps:txbx/w:txbxContent/w:p/w:r/w:rPr/w:sz"

  selected_node <- xml_find_first(x, xpath_, ns = xml_ns( x ))
  expect_equal(xml_attr(selected_node, "val"), "24")
  xpath_ <- ".//wps:wsp/wps:txbx/w:txbxContent/w:p/w:r/w:rPr/w:szCs"
  selected_node <- xml_find_first(x, xpath_, ns = xml_ns( x ))
  expect_equal(xml_attr(selected_node, "val"), "24")
})

test_that("cex does not generate fractional font sizes", {
  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, 0.5, "a", cex = .25)
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  xpath_ <- ".//wps:wsp/wps:txbx/w:txbxContent/w:p/w:r/w:rPr/w:sz"
  selected_node <- xml_find_first(x, xpath_, ns = xml_ns( x ))
  expect_equal(xml_attr(selected_node, "val"), "6")
})

test_that("font sets weight/style", {
  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, .5, "a", font = 1)
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  xpath_b <- ".//wps:wsp/wps:txbx/w:txbxContent/w:p/w:r/w:rPr/w:b"
  xpath_i <- ".//wps:wsp/wps:txbx/w:txbxContent/w:p/w:r/w:rPr/w:i"

  selected_node <- xml_find_first(x, xpath_b, ns = xml_ns( x ))
  expect_is(selected_node, "xml_missing")
  selected_node <- xml_find_first(x, xpath_i, ns = xml_ns( x ))
  expect_is(selected_node, "xml_missing")

  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, .5, "a", font = 2)
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  selected_node <- xml_find_first(x, xpath_b, ns = xml_ns( x ))
  expect_is(selected_node, "xml_node")
  selected_node <- xml_find_first(x, xpath_i, ns = xml_ns( x ))
  expect_is(selected_node, "xml_missing")

  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, .5, "a", font = 3)
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  selected_node <- xml_find_first(x, xpath_b, ns = xml_ns( x ))
  expect_is(selected_node, "xml_missing")
  selected_node <- xml_find_first(x, xpath_i, ns = xml_ns( x ))
  expect_is(selected_node, "xml_node")

  file <- tempfile()
  dml_docx( file = file, bg = "transparent" )
  plot.new()
  text(0.5, .5, "a", font = 4)
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  selected_node <- xml_find_first(x, xpath_b, ns = xml_ns( x ))
  expect_is(selected_node, "xml_node")
  selected_node <- xml_find_first(x, xpath_i, ns = xml_ns( x ))
  expect_is(selected_node, "xml_node")
})



test_that("font sets weight/style", {
  file <- tempfile()
  dml_docx( file = file, bg = "transparent",
            system_fonts = list(sans="Arial", serif = "Times New Roman",
                                mono = "Courier New", symbol = "DejaVu Math TeX Gyre")
  )
  plot.new()
  text(0.5, 0.1, "a", family = "serif")
  text(0.5, 0.5, "a", family = "sans")
  text(0.5, 0.9, "a", family = "mono")
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  xpath_ <- ".//wps:wsp/wps:txbx/w:txbxContent/w:p/w:r/w:rPr/w:rFonts"
  nodes <- xml_find_all(x, xpath_, ns = xml_ns( x ))

  expect_equal(xml_attr(nodes, "ascii"), c("Times New Roman", "Arial", "Courier New"))
  expect_equal(xml_attr(nodes, "hAnsi"), c("Times New Roman", "Arial", "Courier New"))
  expect_equal(xml_attr(nodes, "cs"), c("Times New Roman", "Arial", "Courier New"))
})

test_that("a symbol has width greater than 0", {
  file <- tempfile()
  dml_docx( file = file, bg = "transparent",
            system_fonts = list(symbol = "DejaVu Math TeX Gyre"))
  plot(c(0,2), c(0,2), type = "n")
  strw <- strwidth(expression(symbol("\042")))
  dev.off()

  expect_gt(strw, 0)
})

test_that("symbol font family is 'Symbol'", {
  file <- tempfile()
  dml_docx( file = file, bg = "transparent",
            system_fonts = list(symbol = "DejaVu Math TeX Gyre"))
  plot(c(0,2), c(0,2), type = "n", axes = FALSE, xlab = "", ylab = "")
  text(1, 1, expression(symbol("\042")))
  dev.off()

  x <- read_xml(file)
  ns <-  xml_ns( x )
  xpath_ <- ".//wps:wsp/wps:txbx/w:txbxContent/w:p/w:r/w:rPr/w:rFonts"
  font_node <- xml_find_first(x, xpath_, ns = xml_ns( x ))
  expect_equal(xml_attr(font_node, "ascii"), "DejaVu Math TeX Gyre")
  expect_equal(xml_attr(font_node, "hAnsi"), "DejaVu Math TeX Gyre")
  expect_equal(xml_attr(font_node, "cs"), "DejaVu Math TeX Gyre")
})
