context("XLSX shape dimensions")
library(xml2)

test_that("rect has dimensions", {

  file <- tempfile()

  dml_xlsx( file = file, bg = "transparent" )
  plot.new()
  rect(0.2, 0.2, 0.8, 0.8)
  dev.off()


  doc <- read_xml(file)
  xfrm_node <- xml_find_one(doc, ".//xdr:sp/xdr:spPr/a:xfrm", ns = xml_ns( doc ))
  expect_is(object = xfrm_node, class = "xml_node")

  off_node <- xml_find_one(doc, ".//xdr:sp/xdr:spPr/a:xfrm/a:off", ns = xml_ns( doc ))
  offx <- xml_attr(off_node, "x")
  offy <- xml_attr(off_node, "y")
  expect_true( grepl("^[0-9]+$", offx ) )
  expect_true( grepl("^[0-9]+$", offy ) )

  ext_node <- xml_find_one(doc, ".//xdr:sp/xdr:spPr/a:xfrm/a:ext", ns = xml_ns( doc ))
  cx <- xml_attr(ext_node, "cx")
  cy <- xml_attr(ext_node, "cy")
  expect_true( grepl("^[0-9]+$", cx ) )
  expect_true( grepl("^[0-9]+$", cy ) )
})

test_that("lines has dimensions", {

  file <- tempfile()

  dml_xlsx( file = file, bg = "transparent" )
  plot.new()
  lines(par("usr")[1:2], par("usr")[3:4], lwd = 1)
  dev.off()


  doc <- read_xml(file)
  xfrm_node <- xml_find_one(doc, ".//xdr:sp/xdr:spPr/a:xfrm", ns = xml_ns( doc ))
  expect_is(object = xfrm_node, class = "xml_node")

  off_node <- xml_find_one(doc, ".//xdr:sp/xdr:spPr/a:xfrm/a:off", ns = xml_ns( doc ))
  offx <- xml_attr(off_node, "x")
  offy <- xml_attr(off_node, "y")
  expect_true( grepl("^[0-9]+$", offx ) )
  expect_true( grepl("^[0-9]+$", offy ) )

  ext_node <- xml_find_one(doc, ".//xdr:sp/xdr:spPr/a:xfrm/a:ext", ns = xml_ns( doc ))
  cx <- xml_attr(ext_node, "cx")
  cy <- xml_attr(ext_node, "cy")
  expect_true( grepl("^[0-9]+$", cx ) )
  expect_true( grepl("^[0-9]+$", cy ) )
})

test_that("polygon has dimensions", {

  file <- tempfile()

  dml_xlsx( file = file, bg = "transparent" )
  plot.new()
  x <- par("usr")[1:2]
  y <- par("usr")[3:4]
  x <- c(x[1], x[1], x[2], x[2], NA )
  y <- c(y[2], y[1], y[1], y[2], NA )
  polygon(x, y, lwd = 1)
  dev.off()


  doc <- read_xml(file)
  xfrm_node <- xml_find_one(doc, ".//xdr:sp/xdr:spPr/a:xfrm", ns = xml_ns( doc ))
  expect_is(object = xfrm_node, class = "xml_node")

  off_node <- xml_find_one(doc, ".//xdr:sp/xdr:spPr/a:xfrm/a:off", ns = xml_ns( doc ))
  offx <- xml_attr(off_node, "x")
  offy <- xml_attr(off_node, "y")
  expect_true( grepl("^[0-9]+$", offx ) )
  expect_true( grepl("^[0-9]+$", offy ) )

  ext_node <- xml_find_one(doc, ".//xdr:sp/xdr:spPr/a:xfrm/a:ext", ns = xml_ns( doc ))
  cx <- xml_attr(ext_node, "cx")
  cy <- xml_attr(ext_node, "cy")
  expect_true( grepl("^[0-9]+$", cx ) )
  expect_true( grepl("^[0-9]+$", cy ) )
})


test_that("text has dimensions", {

  file <- tempfile()

  dml_xlsx( file = file, bg = "transparent" )
  plot.new()
  text(x = .5, y = .5, labels = "test")
  dev.off()


  doc <- read_xml(file)
  xfrm_node <- xml_find_one(doc, ".//xdr:sp/xdr:spPr/a:xfrm", ns = xml_ns( doc ))
  expect_is(object = xfrm_node, class = "xml_node")

  off_node <- xml_find_one(doc, ".//xdr:sp/xdr:spPr/a:xfrm/a:off", ns = xml_ns( doc ))
  offx <- xml_attr(off_node, "x")
  offy <- xml_attr(off_node, "y")
  expect_true( grepl("^[0-9]+$", offx ) )
  expect_true( grepl("^[0-9]+$", offy ) )

  ext_node <- xml_find_one(doc, ".//xdr:sp/xdr:spPr/a:xfrm/a:ext", ns = xml_ns( doc ))
  cx <- xml_attr(ext_node, "cx")
  cy <- xml_attr(ext_node, "cy")
  expect_true( grepl("^[0-9]+$", cx ) )
  expect_true( grepl("^[0-9]+$", cy ) )
})

