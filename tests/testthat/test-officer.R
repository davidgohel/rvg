context("add vg in pptx")
library(xml2)
library(officer)

test_that("ph_with", {
  doc <- read_pptx()
  doc <- add_slide(doc, "Title and Content", "Office Theme")
  doc <- ph_with(doc, dml(code = barplot(1:5, col = 2:6)), location = ph_location_type(type = "body"))

  xmldoc <- doc$slide$get_slide(1)$get()
  node <- xml_find_all(xmldoc, "//p:spTree/p:grpSp/*")
  expect_false( inherits(node, "xml_missing") )
})

test_that("ph_remove", {

  my_pres <- read_pptx()
  #slide 1
  my_pres <- add_slide(my_pres, layout = "Title and Content", master = "Office Theme")
  my_pres <- ph_with(my_pres, value = "Hello World", location = ph_location_type(type = "title", newlabel = "ouaoua"))
  my_pres <- ph_with(my_pres, value = "A footer", location = ph_location_type(type = "ftr", newlabel = "ouaoui"))
  my_pres <- ph_with(my_pres, value = format(Sys.Date()), location = ph_location_type(type = "dt", newlabel = "ouaoue"))
  #slide 2
  my_pres <- add_slide(my_pres, layout = "Title and Content")
  my_pres <- ph_with(my_pres, value = dml(code = {boxplot(iris)}), location = ph_location_type("body", newlabel = "chart"))

  example_pptx <- print(my_pres, target = tempfile(fileext = ".pptx"))

  # Does not remove graphic elements currently
  mypres2 <- read_pptx(example_pptx)
  mypres2 <- on_slide(mypres2, index = 1)
  mypres2 <- ph_remove(mypres2, ph_label = "ouaoua")
  mypres2 <- ph_remove(mypres2, ph_label = "ouaoui")
  mypres2 <- ph_remove(mypres2, ph_label = "ouaoue")
  mypres2 <- on_slide(mypres2, index = 2)
  mypres2 <- ph_remove(mypres2, ph_label = "chart")

  ss_1 <- slide_summary(mypres2, index = 1)
  ss_2 <- slide_summary(mypres2, index = 2)

  expect_equal(nrow(ss_1), 0L)
  expect_equal(nrow(ss_2), 0L)
})
