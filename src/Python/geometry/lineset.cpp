// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "Open3D/Geometry/LineSet.h"
#include "Open3D/Geometry/PointCloud.h"
#include "Python/docstring.h"
#include "Python/geometry/geometry.h"
#include "Python/geometry/geometry_trampoline.h"

using namespace open3d;

void pybind_lineset(py::module &m) {
    py::class_<geometry::LineSet, PyGeometry3D<geometry::LineSet>,
               std::shared_ptr<geometry::LineSet>, geometry::Geometry3D>
            lineset(m, "LineSet",
                    "LineSet define a sets of lines in 3D. A typical "
                    "application is to display the point cloud correspondence "
                    "paris.");
    py::detail::bind_default_constructor<geometry::LineSet>(lineset);
    py::detail::bind_copy_functions<geometry::LineSet>(lineset);
    lineset.def("__repr__",
                [](const geometry::LineSet &lineset) {
                    return std::string("geometry::LineSet with ") +
                           std::to_string(lineset.lines_.size()) + " lines.";
                })
            .def(py::self + py::self)
            .def(py::self += py::self)
            .def("has_points", &geometry::LineSet::HasPoints,
                 "Returns ``True`` if the object contains points.")
            .def("has_lines", &geometry::LineSet::HasLines,
                 "Returns ``True`` if the object contains lines.")
            .def("has_colors", &geometry::LineSet::HasColors,
                 "Returns ``True`` if the object's lines contain contain "
                 "colors.")
            .def("get_line_coordinate", &geometry::LineSet::GetLineCoordinate,
                 "line_index"_a)
            .def("paint_uniform_color", &geometry::LineSet::PaintUniformColor,
                 "Assigns each line in the line set the same color.")
            .def_readwrite("points", &geometry::LineSet::points_,
                           "``float64`` array of shape ``(num_points, 3)``, "
                           "use ``numpy.asarray()`` to access data: Points "
                           "coordinates.")
            .def_readwrite("lines", &geometry::LineSet::lines_,
                           "``int`` array of shape ``(num_lines, 2)``, use "
                           "``numpy.asarray()`` to access data: Lines denoted "
                           "by the index of points forming the line.")
            .def_readwrite(
                    "colors", &geometry::LineSet::colors_,
                    "``float64`` array of shape ``(num_lines, 3)``, "
                    "range ``[0, 1]`` , use ``numpy.asarray()`` to access "
                    "data: RGB colors of lines.");
    docstring::ClassMethodDocInject(m, "LineSet", "has_colors");
    docstring::ClassMethodDocInject(m, "LineSet", "has_lines");
    docstring::ClassMethodDocInject(m, "LineSet", "has_points");
    docstring::ClassMethodDocInject(m, "LineSet", "get_line_coordinate",
                                    {{"line_index", "Index of the line."}});
    docstring::ClassMethodDocInject(m, "LineSet", "paint_uniform_color",
                                    {{"color", "Color for the LineSet."}});
}

void pybind_lineset_methods(py::module &m) {
    m.def("create_line_set_from_point_cloud_correspondences",
          &geometry::CreateLineSetFromPointCloudCorrespondences,
          "Factory function to create a LineSet from two pointclouds and a "
          "correspondence set.",
          "cloud0"_a, "cloud1"_a, "correspondences"_a);
    docstring::FunctionDocInject(
            m, "create_line_set_from_point_cloud_correspondences",
            {{"cloud0", "First point cloud."},
             {"cloud1", "Second point cloud."},
             {"correspondences", "Set of correspondences."}});

    m.def("create_line_set_from_triangle_mesh",
          &geometry::CreateLineSetFromTriangleMesh,
          "Factory function to create a LineSet from edges of a triangle mesh.",
          "mesh"_a);
    docstring::FunctionDocInject(m, "create_line_set_from_triangle_mesh",
                                 {{"mesh", "The input triangle mesh."}});
}
