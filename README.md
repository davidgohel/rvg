# rvg package

<!-- badges: start -->
[![R-CMD-check](https://github.com/davidgohel/rvg/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/davidgohel/rvg/actions/workflows/R-CMD-check.yaml)
[![version](https://www.r-pkg.org/badges/version/rvg)](https://CRAN.R-project.org/package=rvg)
![cranlogs](https://cranlogs.r-pkg.org/badges/rvg)
<!-- badges: end -->

## Project Purpose

The `rvg` R package facilitates the creation of Vector Graphics in DrawingML format, specifically for integration into Microsoft PowerPoint and Microsoft Excel documents. This allows for high-quality, editable graphics directly within these applications, enhancing the dynamic nature of reports and presentations.

## Key Functionalities

- **Vector Graphics Generation**: Produces DrawingML vector graphics.
- **PowerPoint Integration**: Outputs graphics for PowerPoint using `dml_pptx` via the `officer` package. Graphics are embedded as editable DrawingML.
- **Excel Integration**: Outputs graphics for Excel using `dml_xlsx` via the `officer` package. Graphics are embedded as editable DrawingML.
- **Editable Graphics**: By default, generated graphics are editable within PowerPoint or Excel.
- **Non-Editable Graphics**: Provides an option (`editable = FALSE`) to render graphics as non-editable.

## Technologies Used

- **R**: The package is written in R.
- **XML**: Generates DrawingML, which is an XML-based format.
- **officer package**: Used for embedding the generated graphics into PowerPoint and Excel files.

## Setup/Installation

You can install the `rvg` package from CRAN or from GitHub.

### CRAN Version (Recommended)

To install the stable version from CRAN, run the following command in your R console:

```r
install.packages("rvg")
```

### Development Version

To get the development version from GitHub, you need the `devtools` package. Run the following commands in your R console:

```r
if (!requireNamespace("devtools", quietly = TRUE)) {
  install.packages("devtools")
}
devtools::install_github("davidgohel/rvg")
```

## Usage Instructions

The `rvg` package provides functions to create DrawingML objects that can then be added to PowerPoint presentations or Excel spreadsheets using the `officer` package.

### For PowerPoint

Use the `dml()` function to wrap your plot code and then add it to a PowerPoint slide using `ph_with()` from the `officer` package.

```r
library(rvg)
library(officer)
library(ggplot2) # Example using ggplot2

# Create a plot and wrap it with dml()
my_plot_code <- ggplot(data = iris, aes(x = Sepal.Length, y = Sepal.Width, color = Species)) +
  geom_point() +
  ggtitle("Iris Sepal Length vs. Width")

my_vec_graph <- dml(code = print(my_plot_code))

# Create a PowerPoint document and add the graphic
doc <- read_pptx()
doc <- add_slide(doc, layout = "Title and Content", master = "Office Theme")
doc <- ph_with(doc, my_vec_graph, location = ph_location_fullsize())
print(doc, target = "my_plot_presentation.pptx")
```

### For Excel

Use the `xl_add_vg()` function (which is part of `officer`, but works with `rvg` outputs) to add a plot to an Excel sheet. Note that `rvg` itself doesn't have `xl_add_vg`, this function comes from `officer` but is designed to work with `rvg`. The example in the original README `doc <- xl_add_vg(doc, sheet = "Feuil1", code = print(gg), ...)` implies `gg` is a plot object.

```r
library(rvg)
library(officer)
library(ggplot2) # Example using ggplot2

# Create a ggplot object
gg_plot_object <- ggplot(data = mtcars, aes(x = mpg, y = wt)) +
  geom_point() +
  ggtitle("MTCars MPG vs. Weight")

# Create an Excel workbook and add the graphic
# Note: Ensure you have an Excel file or create one.
# For this example, let's assume 'existing_workbook.xlsx' exists or is created by read_xlsx()
# If read_xlsx() is used without a path, it creates a new workbook.
xlsx_doc <- read_xlsx()
xlsx_doc <- xl_add_vg(xlsx_doc, sheet = "Sheet1", code = print(gg_plot_object),
                      width = 6, height = 5, left = 1, top = 1)
print(xlsx_doc, target = "my_plot_workbook.xlsx")
```

By default, these graphics are editable. To disable element edition, use the `editable = FALSE` option within the `dml()` function:

```r
my_non_editable_graph <- dml(code = barplot(1:5, col = 2:6), editable = FALSE)
```

## High-level Architecture

The `rvg` package acts as a graphics device for R. When a plot is generated using `rvg` (e.g., via the `dml()` function):

1.  R plotting commands are captured.
2.  `rvg` translates these commands into DrawingML (an XML-based format used by Microsoft Office).
3.  The resulting DrawingML output is then passed to functions from the `officer` package (e.g., `ph_with()` for PowerPoint, `xl_add_vg()` for Excel).
4.  The `officer` package embeds this DrawingML into the .pptx or .xlsx file, making the vector graphic appear in the document.

This architecture allows R users to leverage the native vector graphics capabilities of Microsoft Office, ensuring high fidelity and editability of plots.
