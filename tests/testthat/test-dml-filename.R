context("MS absolute filename are returned")
library(xml2)

test_that("docx filename", {

  file_abs_path <- try( write_docx(file = "my_plot.docx", code = plot(rnorm(10))), silent = TRUE )
  expected <- file.path(getwd(), "my_plot.docx")
  expect_equal(object = file_abs_path, expected = expected )

  file_abs_path <- try( write_docx(file = expected, code = plot(rnorm(10))), silent = TRUE )
  expect_equal(object = file_abs_path, expected = expected )
})

test_that("pptx filename", {

  file_abs_path <- try( write_pptx(file = "my_plot.pptx", code = plot(rnorm(10))), silent = TRUE )
  expected <- file.path(getwd(), "my_plot.pptx")
  expect_equal(object = file_abs_path, expected = expected )

  file_abs_path <- try( write_pptx(file = expected, code = plot(rnorm(10))), silent = TRUE )
  expect_equal(object = file_abs_path, expected = expected )
})

unlink(list.files(pattern = "^my_plot"), force = TRUE)
