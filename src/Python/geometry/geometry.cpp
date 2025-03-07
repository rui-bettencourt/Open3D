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

#include "Python/geometry/geometry.h"

#include "Open3D/Open3D.h"
#include "Python/docstring.h"
#include "Python/geometry/geometry_trampoline.h"
using namespace open3d;

void pybind_geometry_classes(py::module &m) {
    // open3d.geometry.Geometry
    py::class_<geometry::Geometry, PyGeometry<geometry::Geometry>,
               std::shared_ptr<geometry::Geometry>>
            geometry(m, "Geometry", "The base geometry class.");
    geometry.def("clear", &geometry::Geometry::Clear,
                 "Clear all elements in the geometry.")
            .def("is_empty", &geometry::Geometry::IsEmpty,
                 "Returns ``True`` iff the geometry is empty.")
            .def("get_geometry_type", &geometry::Geometry::GetGeometryType,
                 "Returns one of registered geometry types.")
            .def("dimension", &geometry::Geometry::Dimension,
                 "Returns whether the geometry is 2D or 3D.");
    docstring::ClassMethodDocInject(m, "Geometry", "clear");
    docstring::ClassMethodDocInject(m, "Geometry", "is_empty");
    docstring::ClassMethodDocInject(m, "Geometry", "get_geometry_type");
    docstring::ClassMethodDocInject(m, "Geometry", "dimension");

    // open3d.geometry.Geometry.Type
    py::enum_<geometry::Geometry::GeometryType> geometry_type(geometry, "Type",
                                                              py::arithmetic());
    // Trick to write docs without listing the members in the enum class again.
    geometry_type.attr("__doc__") = docstring::static_property(
            py::cpp_function([](py::handle arg) -> std::string {
                return "Enum class for Geometry types.";
            }),
            py::none(), py::none(), "");

    geometry_type
            .value("Unspecified", geometry::Geometry::GeometryType::Unspecified)
            .value("PointCloud", geometry::Geometry::GeometryType::PointCloud)
            .value("VoxelGrid", geometry::Geometry::GeometryType::VoxelGrid)
            .value("LineSet", geometry::Geometry::GeometryType::LineSet)
            .value("TriangleMesh",
                   geometry::Geometry::GeometryType::TriangleMesh)
            .value("HalfEdgeTriangleMesh",
                   geometry::Geometry::GeometryType::HalfEdgeTriangleMesh)
            .value("Image", geometry::Geometry::GeometryType::Image)
            .export_values();

    // open3d.geometry.Geometry3D
    py::enum_<geometry::Geometry3D::RotationType>(m, "RotationType")
            .value("XYZ", geometry::Geometry3D::RotationType::XYZ)
            .value("YZX", geometry::Geometry3D::RotationType::YZX)
            .value("ZXY", geometry::Geometry3D::RotationType::ZXY)
            .value("XZY", geometry::Geometry3D::RotationType::XZY)
            .value("ZYX", geometry::Geometry3D::RotationType::ZYX)
            .value("YXZ", geometry::Geometry3D::RotationType::YXZ)
            .value("AxisAngle", geometry::Geometry3D::RotationType::AxisAngle)
            .export_values();

    py::class_<geometry::Geometry3D, PyGeometry3D<geometry::Geometry3D>,
               std::shared_ptr<geometry::Geometry3D>, geometry::Geometry>
            geometry3d(m, "Geometry3D",
                       "The base geometry class for 3D geometries.");
    geometry3d
            .def("get_min_bound", &geometry::Geometry3D::GetMinBound,
                 "Returns min bounds for geometry coordinates.")
            .def("get_max_bound", &geometry::Geometry3D::GetMaxBound,
                 "Returns max bounds for geometry coordinates.")
            .def("transform", &geometry::Geometry3D::Transform,
                 "Apply transformation (4x4 matrix) to the geometry "
                 "coordinates.")
            .def("translate", &geometry::Geometry3D::Translate,
                 "Apply translation to the geometry coordinates.")
            .def("scale", &geometry::Geometry3D::Scale,
                 "Apply scaling to the geometry coordinates.", "scale"_a,
                 "center"_a = true)
            .def("rotate", &geometry::Geometry3D::Rotate,
                 "Apply rotation to the geometry coordinates and normals.",
                 "rotation"_a, "center"_a = true,
                 "type"_a = geometry::Geometry3D::RotationType::XYZ);
    docstring::ClassMethodDocInject(m, "Geometry3D", "get_min_bound");
    docstring::ClassMethodDocInject(m, "Geometry3D", "get_max_bound");
    docstring::ClassMethodDocInject(m, "Geometry3D", "transform");
    docstring::ClassMethodDocInject(m, "Geometry3D", "translate");
    docstring::ClassMethodDocInject(
            m, "Geometry3D", "scale",
            {{"scale",
              "The scale parameter that is multiplied to the points/vertices "
              "of the geometry"},
             {"center",
              "If true, then the scale is applied to the centered geometry"}});
    docstring::ClassMethodDocInject(
            m, "Geometry3D", "rotate",
            {{"rotation",
              "A 3D vector that either defines the three angles for "
              "Euler rotation, or in the axis-angle representation "
              "the normalized vector defines the axis of rotation and "
              "the norm the angle around this axis."},
             {"center",
              "If true, then the rotation is applied to the centered geometry"},
             {"type",
              "Type of rotation, i.e., an Euler format, or "
              "axis-angle."}});

    // open3d.geometry.Geometry2D
    py::class_<geometry::Geometry2D, PyGeometry2D<geometry::Geometry2D>,
               std::shared_ptr<geometry::Geometry2D>, geometry::Geometry>
            geometry2d(m, "Geometry2D",
                       "The base geometry class for 2D geometries.");
    geometry2d
            .def("get_min_bound", &geometry::Geometry2D::GetMinBound,
                 "Returns min bounds for geometry coordinates.")
            .def("get_max_bound", &geometry::Geometry2D::GetMaxBound,
                 "Returns max bounds for geometry coordinates.");
    docstring::ClassMethodDocInject(m, "Geometry2D", "get_min_bound");
    docstring::ClassMethodDocInject(m, "Geometry2D", "get_max_bound");
}

void pybind_geometry(py::module &m) {
    py::module m_submodule = m.def_submodule("geometry");
    pybind_geometry_classes(m_submodule);
    pybind_pointcloud(m_submodule);
    pybind_voxelgrid(m_submodule);
    pybind_lineset(m_submodule);
    pybind_trianglemesh(m_submodule);
    pybind_halfedgetrianglemesh(m_submodule);
    pybind_image(m_submodule);
    pybind_kdtreeflann(m_submodule);
    pybind_pointcloud_methods(m_submodule);
    pybind_voxelgrid_methods(m_submodule);
    pybind_trianglemesh_methods(m_submodule);
    pybind_lineset_methods(m_submodule);
    pybind_image_methods(m_submodule);
    pybind_octree_methods(m_submodule);
    pybind_octree(m_submodule);
}
