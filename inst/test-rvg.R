library(tidyverse)
library(rvg)
library(officer)


# plot <- ggplot(mtcars, aes(x = disp, y = mpg)) +
#   geom_point()
#
# plot_rvg <- dml(ggobj = plot)
#
# ppt_template <- read_pptx("new_template.pptx") %>%
#   add_slide(layout = "standard", master = "default") %>%
#   ph_with(plot, location = ph_location(left = 1.09, top = 3.03, height = 3.7, width = 9.4)) %>%
#   add_slide(layout = "standard", master = "default") %>%
#   ph_with(plot_rvg, location = ph_location(left = 1.09, top = 3.03, height = 3.7, width = 9.4))
#
# print(ppt_template, "sample_report_template.pptx")
#
#
# ppt_default <- read_pptx() %>%
#   add_slide(layout = "Title and Content", master = "Office Theme") %>%
#   ph_with(plot_rvg, location = ph_location_type())
#
# print(ppt_default, "sample_report_default.pptx")


# -- Test compound paths (donut with hole) ------------------------------------
# Uses polypath with EvenOdd rule: outer circle filled, inner circle cut out

angles <- seq(0, 2 * pi, length.out = 100)[-1]
donut <- data.frame(
  x = c(cos(angles) * 3, cos(rev(angles)) * 1.5),
  y = c(sin(angles) * 3, sin(rev(angles)) * 1.5),
  group = 1
)

plot_path <- ggplot(donut, aes(x, y, group = group)) +
  geom_polygon(fill = "steelblue", colour = "black") +
  # polypath-based donut via geom_polygon with a hole subgroup
  coord_equal() +
  theme_minimal() +
  ggtitle("Compound path: donut (hole should be visible)")

# Alternative using explicit path with geom from ggforce
# or a simpler polygon-with-hole example:
plot_path2 <- ggplot() +
  geom_polygon(
    data = data.frame(
      x = c(0, 10, 10, 0, 3, 3, 7, 7),
      y = c(0, 0, 10, 10, 3, 7, 7, 3),
      subid = c(rep(1, 4), rep(2, 4))
    ),
    aes(x = x, y = y, subgroup = subid),
    fill = "coral", colour = "black"
  ) +
  coord_equal() +
  theme_minimal() +
  ggtitle("Compound path: square with hole")

# PPTX output
pptx_path_test <- read_pptx() %>%
  add_slide(layout = "Title and Content", master = "Office Theme") %>%
  ph_with(dml(ggobj = plot_path), location = ph_location_type()) %>%
  add_slide(layout = "Title and Content", master = "Office Theme") %>%
  ph_with(dml(ggobj = plot_path2), location = ph_location_type())

print(pptx_path_test, "test_compound_path.pptx")

# XLSX output
wb <- read_xlsx()
wb <- xl_add_vg(wb, sheet = "Feuil1",
  code = print(plot_path2),
  width = 6, height = 6, left = 1, top = 1)
print(wb, "test_compound_path.xlsx")

