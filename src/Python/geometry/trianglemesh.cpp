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

#include "Open3D/Geometry/TriangleMesh.h"
#include "Open3D/Geometry/PointCloud.h"
#include "Python/docstring.h"
#include "Python/geometry/geometry.h"
#include "Python/geometry/geometry_trampoline.h"

using namespace open3d;

void pybind_trianglemesh(py::module &m) {
    py::class_<geometry::TriangleMesh, PyGeometry3D<geometry::TriangleMesh>,
               std::shared_ptr<geometry::TriangleMesh>, geometry::Geometry3D>
            trianglemesh(m, "TriangleMesh",
                         "TriangleMesh class. Triangle mesh contains vertices "
                         "and triangles represented by the indices to the "
                         "vertices. Optionally, the mesh may also contain "
                         "triangle normals, vertex normals and vertex colors.");
    py::detail::bind_default_constructor<geometry::TriangleMesh>(trianglemesh);
    py::detail::bind_copy_functions<geometry::TriangleMesh>(trianglemesh);
    py::enum_<geometry::TriangleMesh::SimplificationContraction>(
            m, "SimplificationContraction")
            .value("Average",
                   geometry::TriangleMesh::SimplificationContraction::Average,
                   "The vertex positions are computed by the averaging.")
            .value("Quadric",
                   geometry::TriangleMesh::SimplificationContraction::Quadric,
                   "The vertex positions are computed by minimizing the "
                   "distance to the adjacent triangle planes.")
            .export_values();
    py::enum_<geometry::TriangleMesh::FilterScope>(m, "FilterScope")
            .value("All", geometry::TriangleMesh::FilterScope::All,
                   "All properties (color, normal, vertex position) are "
                   "filtered.")
            .value("Color", geometry::TriangleMesh::FilterScope::Color,
                   "Only the color values are filtered.")
            .value("Normal", geometry::TriangleMesh::FilterScope::Normal,
                   "Only the normal values are filtered.")
            .value("Vertex", geometry::TriangleMesh::FilterScope::Vertex,
                   "Only the vertex positions are filtered.")
            .export_values();
    trianglemesh
            .def("__repr__",
                 [](const geometry::TriangleMesh &mesh) {
                     return std::string("geometry::TriangleMesh with ") +
                            std::to_string(mesh.vertices_.size()) +
                            " points and " +
                            std::to_string(mesh.triangles_.size()) +
                            " triangles.";
                 })
            .def(py::self + py::self)
            .def(py::self += py::self)
            .def("compute_triangle_normals",
                 &geometry::TriangleMesh::ComputeTriangleNormals,
                 "Function to compute triangle normals, usually called before "
                 "rendering",
                 "normalized"_a = true)
            .def("compute_vertex_normals",
                 &geometry::TriangleMesh::ComputeVertexNormals,
                 "Function to compute vertex normals, usually called before "
                 "rendering",
                 "normalized"_a = true)
            .def("compute_adjacency_list",
                 &geometry::TriangleMesh::ComputeAdjacencyList,
                 "Function to compute adjacency list, call before adjacency "
                 "list is needed")
            .def("remove_duplicated_vertices",
                 &geometry::TriangleMesh::RemoveDuplicatedVertices,
                 "Function that removes duplicated verties, i.e., vertices "
                 "that have identical coordinates.")
            .def("remove_duplicated_triangles",
                 &geometry::TriangleMesh::RemoveDuplicatedTriangles,
                 "Function that removes duplicated triangles, i.e., removes "
                 "triangles that reference the same three vertices, "
                 "independent of their order.")
            .def("remove_unreferenced_vertices",
                 &geometry::TriangleMesh::RemoveUnreferencedVertices,
                 "This function removes vertices from the triangle mesh that "
                 "are not referenced in any triangle of the mesh.")
            .def("remove_degenerate_triangles",
                 &geometry::TriangleMesh::RemoveDegenerateTriangles,
                 "Function that removes degenerate triangles, i.e., triangles "
                 "that references a single vertex multiple times in a single "
                 "triangle. They are usually the product of removing "
                 "duplicated vertices.")
            .def("remove_non_manifold_edges",
                 &geometry::TriangleMesh::RemoveNonManifoldEdges,
                 "Function that removes all non-manifold edges, by "
                 "successively deleting  triangles with the smallest surface "
                 "area adjacent to the non-manifold edge until the number of "
                 "adjacent triangles to the edge is `<= 2`.")
            .def("filter_sharpen", &geometry::TriangleMesh::FilterSharpen,
                 "Function to sharpen triangle mesh. The output value "
                 "(:math:`v_o`) is the input value (:math:`v_i`) plus strength "
                 "times the input value minus he sum of he adjacent values. "
                 ":math:`v_o = v_i x strength (v_i * |N| - \\sum_{n \\in N} "
                 "v_n)`",
                 "number_of_iterations"_a = 1, "strength"_a = 1,
                 "filter_scope"_a = geometry::TriangleMesh::FilterScope::All)
            .def("filter_smooth_simple",
                 &geometry::TriangleMesh::FilterSmoothSimple,
                 "Function to smooth triangle mesh with simple neighbour "
                 "average. :math:`v_o = \\frac{v_i + \\sum_{n \\in N} "
                 "v_n)}{|N| + 1}`, with :math:`v_i` being the input value, "
                 ":math:`v_o` the output value, and :math:`N` is the set of "
                 "adjacent neighbours.",
                 "number_of_iterations"_a = 1,
                 "filter_scope"_a = geometry::TriangleMesh::FilterScope::All)
            .def("filter_smooth_laplacian",
                 &geometry::TriangleMesh::FilterSmoothLaplacian,
                 "Function to smooth triangle mesh using Laplacian. :math:`v_o "
                 "= v_i \\cdot \\lambda (sum_{n \\in N} w_n v_n - v_i)`, with "
                 ":math:`v_i` being the input value, :math:`v_o` the output "
                 "value, :math:`N` is the  set of adjacent neighbours, "
                 ":math:`w_n` is the weighting of the neighbour based on the "
                 "inverse distance (closer neighbours have higher weight), and "
                 "lambda is the smoothing parameter.",
                 "number_of_iterations"_a = 1, "lambda"_a = 0.5,
                 "filter_scope"_a = geometry::TriangleMesh::FilterScope::All)
            .def("filter_smooth_taubin",
                 &geometry::TriangleMesh::FilterSmoothTaubin,
                 "Function to smooth triangle mesh using method of Taubin, "
                 "\"Curve and Surface Smoothing Without Shrinkage\", 1995. "
                 "Applies in each iteration two times filter_smooth_laplacian, "
                 "first with filter parameter lambda and second with filter "
                 "parameter mu as smoothing parameter. This method avoids "
                 "shrinkage of the triangle mesh.",
                 "number_of_iterations"_a = 1, "lambda"_a = 0.5, "mu"_a = -0.53,
                 "filter_scope"_a = geometry::TriangleMesh::FilterScope::All)
            .def("has_vertices", &geometry::TriangleMesh::HasVertices,
                 "Returns ``True`` if the mesh contains vertices.")
            .def("has_triangles", &geometry::TriangleMesh::HasTriangles,
                 "Returns ``True`` if the mesh contains triangles.")
            .def("has_vertex_normals",
                 &geometry::TriangleMesh::HasVertexNormals,
                 "Returns ``True`` if the mesh contains vertex normals.")
            .def("has_vertex_colors", &geometry::TriangleMesh::HasVertexColors,
                 "Returns ``True`` if the mesh contains vertex colors.")
            .def("has_triangle_normals",
                 &geometry::TriangleMesh::HasTriangleNormals,
                 "Returns ``True`` if the mesh contains triangle normals.")
            .def("has_adjacency_list",
                 &geometry::TriangleMesh::HasAdjacencyList,
                 "Returns ``True`` if the mesh contains adjacency normals.")
            .def("normalize_normals", &geometry::TriangleMesh::NormalizeNormals,
                 "Normalize both triangle normals and vertex normals to legnth "
                 "1.")
            .def("paint_uniform_color",
                 &geometry::TriangleMesh::PaintUniformColor,
                 "Assigns each vertex in the TriangleMesh the same color.")
            .def("euler_poincare_characteristic",
                 &geometry::TriangleMesh::EulerPoincareCharacteristic,
                 "Function that computes the Euler-Poincaré characteristic, "
                 "i.e., V + F - E, where V is the number of vertices, F is the "
                 "number of triangles, and E is the number of edges.")
            .def("get_non_manifold_edges",
                 &geometry::TriangleMesh::GetNonManifoldEdges,
                 "Get list of non-manifold edges.",
                 "allow_boundary_edges"_a = true)
            .def("is_edge_manifold", &geometry::TriangleMesh::IsEdgeManifold,
                 "Tests if the triangle mesh is edge manifold.",
                 "allow_boundary_edges"_a = true)
            .def("get_non_manifold_vertices",
                 &geometry::TriangleMesh::GetNonManifoldVertices,
                 "Returns a list of indices to non-manifold vertices.")
            .def("is_vertex_manifold",
                 &geometry::TriangleMesh::IsVertexManifold,
                 "Tests if all vertices of the triangle mesh are manifold.")
            .def("is_self_intersecting",
                 &geometry::TriangleMesh::IsSelfIntersecting,
                 "Tests the triangle mesh is self-intersecting")
            .def("get_self_intersecting_triangles",
                 &geometry::TriangleMesh::GetSelfIntersectingTriangles,
                 "Returns a list of indices to triangles that intersect the "
                 "mesh.")
            .def("is_intersecting", &geometry::TriangleMesh::IsIntersecting,
                 "Tests the triangle mesh is intersecting the other triangle "
                 "mesh.")
            .def("is_orientable", &geometry::TriangleMesh::IsOrientable,
                 "Tests the triangle mesh is orientable")
            .def("orient_triangles", &geometry::TriangleMesh::OrientTriangles,
                 "If the mesh is orientable this function orients all "
                 "triangles such that all normals point towards the same "
                 "direction.")
            .def_readwrite("vertices", &geometry::TriangleMesh::vertices_,
                           "``float64`` array of shape ``(num_vertices, 3)``, "
                           "use ``numpy.asarray()`` to access data: Vertex "
                           "coordinates.")
            .def_readwrite("vertex_normals",
                           &geometry::TriangleMesh::vertex_normals_,
                           "``float64`` array of shape ``(num_vertices, 3)``, "
                           "use ``numpy.asarray()`` to access data: Vertex "
                           "normals.")
            .def_readwrite(
                    "vertex_colors", &geometry::TriangleMesh::vertex_colors_,
                    "``float64`` array of shape ``(num_vertices, 3)``, "
                    "range ``[0, 1]`` , use ``numpy.asarray()`` to access "
                    "data: RGB colors of vertices.")
            .def_readwrite("triangles", &geometry::TriangleMesh::triangles_,
                           "``int`` array of shape ``(num_triangles, 3)``, use "
                           "``numpy.asarray()`` to access data: List of "
                           "triangles denoted by the index of points forming "
                           "the triangle.")
            .def_readwrite("triangle_normals",
                           &geometry::TriangleMesh::triangle_normals_,
                           "``float64`` array of shape ``(num_triangles, 3)``, "
                           "use ``numpy.asarray()`` to access data: Triangle "
                           "normals.")
            .def_readwrite(
                    "adjacency_list", &geometry::TriangleMesh::adjacency_list_,
                    "List of Sets: The set ``adjacency_list[i]`` contains the "
                    "indices of adjacent vertices of vertex i.");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "compute_adjacency_list");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "compute_triangle_normals");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "compute_vertex_normals");
    docstring::ClassMethodDocInject(m, "TriangleMesh", "has_adjacency_list");
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "has_triangle_normals",
            {{"normalized",
              "Set to ``True`` to normalize the normal to length 1."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh", "has_triangles");
    docstring::ClassMethodDocInject(m, "TriangleMesh", "has_vertex_colors");
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "has_vertex_normals",
            {{"normalized",
              "Set to ``True`` to normalize the normal to length 1."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh", "has_vertices");
    docstring::ClassMethodDocInject(m, "TriangleMesh", "normalize_normals");
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "paint_uniform_color",
            {{"color", "RGB color for the PointCloud."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "euler_poincare_characteristic");
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "get_non_manifold_edges",
            {{"allow_boundary_edges",
              "If true, than non-manifold edges are defined as edges with more "
              "than two adjacent triangles, otherwise each edge that is not "
              "adjacent to two triangles is defined as non-manifold."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "is_edge_manifold",
            {{"allow_boundary_edges",
              "If true, than non-manifold edges are defined as edges with more "
              "than two adjacent triangles, otherwise each edge that is not "
              "adjacent to two triangles is defined as non-manifold."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh", "is_vertex_manifold");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "get_non_manifold_vertices");
    docstring::ClassMethodDocInject(m, "TriangleMesh", "is_self_intersecting");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "get_self_intersecting_triangles");
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "is_intersecting",
            {{"other", "Other triangle mesh to test intersection with."}});
    docstring::ClassMethodDocInject(m, "TriangleMesh", "is_orientable");
    docstring::ClassMethodDocInject(m, "TriangleMesh", "orient_triangles");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "remove_duplicated_vertices");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "remove_duplicated_triangles");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "remove_unreferenced_vertices");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "remove_degenerate_triangles");
    docstring::ClassMethodDocInject(m, "TriangleMesh",
                                    "remove_non_manifold_edges");
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "filter_sharpen",
            {{"number_of_iterations",
              " Number of repetitions of this operation"},
             {"strengh", "Filter parameter."},
             {"scope", "Mesh property that should be filtered."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "filter_smooth_simple",
            {{"number_of_iterations",
              " Number of repetitions of this operation"},
             {"scope", "Mesh property that should be filtered."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "filter_smooth_laplacian",
            {{"number_of_iterations",
              " Number of repetitions of this operation"},
             {"lambda", "Filter parameter."},
             {"scope", "Mesh property that should be filtered."}});
    docstring::ClassMethodDocInject(
            m, "TriangleMesh", "filter_smooth_taubin",
            {{"number_of_iterations",
              " Number of repetitions of this operation"},
             {"lambda", "Filter parameter."},
             {"mu", "Filter parameter."},
             {"scope", "Mesh property that should be filtered."}});
}

void pybind_trianglemesh_methods(py::module &m) {
    // Overloaded function, do not inject docs. Keep commented out for future.
    m.def("select_down_sample",
          (std::shared_ptr<geometry::TriangleMesh>(*)(
                  const geometry::TriangleMesh &,
                  const std::vector<size_t> &)) &
                  geometry::SelectDownSample,
          "Function to select mesh from input triangle mesh into output "
          "triangle mesh. ``input``: The input triangle mesh. ``indices``: "
          "Indices of vertices to be selected.",
          "input"_a, "indices"_a);
    // docstring::FunctionDocInject(
    //         m, "select_down_sample",
    //         {{"input", "The input triangle mesh."},
    //          {"indices", "Indices of vertices to be selected."}});

    m.def("crop_triangle_mesh", &geometry::CropTriangleMesh,
          "Function to crop input triangle mesh into output triangle mesh",
          "input"_a, "min_bound"_a, "max_bound"_a);
    docstring::FunctionDocInject(
            m, "crop_triangle_mesh",
            {{"input", "The input triangle mesh."},
             {"min_bound", "Minimum bound for vertex coordinate."},
             {"max_bound", "Maximum bound for vertex coordinate."}});

    m.def("sample_points_uniformly", &geometry::SamplePointsUniformly,
          "Function to uniformly sample points from the mesh.", "input"_a,
          "number_of_points"_a = 100);
    docstring::FunctionDocInject(
            m, "sample_points_uniformly",
            {{"input", "The input triangle mesh."},
             {"number_of_points",
              "Number of points that should be uniformly sampled."}});

    m.def("sample_points_poisson_disk", &geometry::SamplePointsPoissonDisk,
          "Function to sample points from the mesh, where each point has "
          "approximately the same distance to the neighbouring points (blue "
          "noise). Method is based on Yuksel, \"Sample Elimination for "
          "Generating Poisson Disk Sample Sets\", EUROGRAPHICS, 2015.",
          "input"_a, "number_of_points"_a, "init_factor"_a = 5,
          "pcl"_a = nullptr);
    docstring::FunctionDocInject(
            m, "sample_points_poisson_disk",
            {{"input", "The input triangle mesh."},
             {"number_of_points", "Number of points that should be sampled."},
             {"init_factor",
              "Factor for the initial uniformly sampled PointCloud. This init "
              "PointCloud is used for sample elimination."},
             {"pcl",
              "Initial PointCloud that is used for sample elimination. If this "
              "parameter is provided the init_factor is ignored."}});

    m.def("subdivide_midpoint", &geometry::SubdivideMidpoint,
          "Function subdivide mesh using midpoint algorithm.", "input"_a,
          "number_of_iterations"_a = 1);
    docstring::FunctionDocInject(
            m, "subdivide_midpoint",
            {{"input", "The input triangle mesh."},
             {"number_of_iterations",
              "Number of iterations. A single iteration splits each triangle "
              "into four triangles that cover the same surface."}});

    m.def("subdivide_loop", &geometry::SubdivideLoop,
          "Function subdivide mesh using Loop's algorithm. Loop, \"Smooth "
          "subdivision surfaces based on triangles\", 1987.",
          "input"_a, "number_of_iterations"_a = 1);
    docstring::FunctionDocInject(
            m, "subdivide_loop",
            {{"input", "The input triangle mesh."},
             {"number_of_iterations",
              "Number of iterations. A single iteration splits each triangle "
              "into four triangles."}});

    m.def("simplify_vertex_clustering", &geometry::SimplifyVertexClustering,
          "Function to simplify mesh using vertex clustering.", "input"_a,
          "voxel_size"_a,
          "contraction"_a =
                  geometry::TriangleMesh::SimplificationContraction::Average);
    docstring::FunctionDocInject(
            m, "simplify_vertex_clustering",
            {{"input", "The input triangle mesh."},
             {"voxel_size",
              "The size of the voxel within vertices are pooled."},
             {"contraction",
              "Method to aggregate vertex information. Average computes a "
              "simple average, Quadric minimizes the distance to the adjacent "
              "planes."}});
    m.def("simplify_quadric_decimation", &geometry::SimplifyQuadricDecimation,
          "Function to simplify mesh using Quadric Error Metric Decimation by "
          "Garland and Heckbert",
          "input"_a, "target_number_of_triangles"_a);
    docstring::FunctionDocInject(
            m, "simplify_quadric_decimation",
            {{"input", "The input triangle mesh."},
             {"target_number_of_triangles",
              "The number of triangles that the simplified mesh should have. "
              "It is not guranteed that this number will be reached."}});

    m.def("compute_mesh_convex_hull", &geometry::ComputeMeshConvexHull,
          "Computes the convex hull of the triangle mesh.", "input"_a);
    docstring::FunctionDocInject(m, "compute_mesh_convex_hull",
                                 {{"input", "The input triangle mesh."}});

    m.def("create_mesh_box", &geometry::CreateMeshBox,
          "Factory function to create a box. The left bottom corner on the "
          "front will be placed at (0, 0, 0).",
          "width"_a = 1.0, "height"_a = 1.0, "depth"_a = 1.0);
    docstring::FunctionDocInject(m, "create_mesh_box",
                                 {{"width", "x-directional length."},
                                  {"height", "y-directional length."},
                                  {"depth", "z-directional length."}});

    m.def("create_mesh_tetrahedron", &geometry::CreateMeshTetrahedron,
          "Factory function to create a tetrahedron. The centroid of the mesh "
          "will be placed at (0, 0, 0) and the vertices have a distance of "
          "radius to the center.",
          "radius"_a = 1.0);
    docstring::FunctionDocInject(
            m, "create_mesh_tetrahedron",
            {{"radius", "Distance from centroid to mesh vetices."}});

    m.def("create_mesh_octahedron", &geometry::CreateMeshOctahedron,
          "Factory function to create a octahedron. The centroid of the mesh "
          "will be placed at (0, 0, 0) and the vertices have a distance of "
          "radius to the center.",
          "radius"_a = 1.0);
    docstring::FunctionDocInject(
            m, "create_mesh_octahedron",
            {{"radius", "Distance from centroid to mesh vetices."}});

    m.def("create_mesh_icosahedron", &geometry::CreateMeshIcosahedron,
          "Factory function to create a icosahedron. The centroid of the mesh "
          "will be placed at (0, 0, 0) and the vertices have a distance of "
          "radius to the center.",
          "radius"_a = 1.0);
    docstring::FunctionDocInject(
            m, "create_mesh_icosahedron",
            {{"radius", "Distance from centroid to mesh vetices."}});

    m.def("create_mesh_sphere", &geometry::CreateMeshSphere,
          "Factory function to create a sphere mesh centered at (0, 0, 0).",
          "radius"_a = 1.0, "resolution"_a = 20);
    docstring::FunctionDocInject(
            m, "create_mesh_sphere",
            {{"radius", "The radius of the sphere."},
             {"resolution",
              "The resolution of the sphere. The longitues will be split into "
              "``resolution`` segments (i.e. there are ``resolution + 1`` "
              "latitude lines including the north and south pole). The "
              "latitudes will be split into ```2 * resolution`` segments (i.e. "
              "there are ``2 * resolution`` longitude lines.)"}});

    m.def("create_mesh_cylinder", &geometry::CreateMeshCylinder,
          "Factory function to create a cylinder mesh.", "radius"_a = 1.0,
          "height"_a = 2.0, "resolution"_a = 20, "split"_a = 4);
    docstring::FunctionDocInject(
            m, "create_mesh_cylinder",
            {{"radius", "The radius of the cylinder."},
             {"height",
              "The height of the cylinder. The axis of the cylinder will be "
              "from (0, 0, -height/2) to (0, 0, height/2)."},
             {"resolution",
              " The circle will be split into ``resolution`` segments"},
             {"split",
              "The ``height`` will be split into ``split`` segments."}});

    m.def("create_mesh_cone", &geometry::CreateMeshCone,
          "Factory function to create a cone mesh.", "radius"_a = 1.0,
          "height"_a = 2.0, "resolution"_a = 20, "split"_a = 1);
    docstring::FunctionDocInject(
            m, "create_mesh_cone",
            {{"radius", "The radius of the cone."},
             {"height",
              "The height of the cone. The axis of the cone will be from (0, "
              "0, 0) to (0, 0, height)."},
             {"resolution",
              "The circle will be split into ``resolution`` segments"},
             {"split",
              "The ``height`` will be split into ``split`` segments."}});

    m.def("create_mesh_torus", &geometry::CreateMeshTorus,
          "Factory function to create a torus mesh.", "torus_radius"_a = 1.0,
          "tube_radius"_a = 0.5, "radial_resolution"_a = 30,
          "tubular_resolution"_a = 20);
    docstring::FunctionDocInject(
            m, "create_mesh_torus",
            {{"torus_radius",
              "The radius from the center of the torus to the center of the "
              "tube."},
             {"tube_radius", "The radius of the torus tube."},
             {"radial_resolution",
              "The number of segments along the radial direction."},
             {"tubular_resolution",
              "The number of segments along the tubular direction."}});

    m.def("create_mesh_arrow", &geometry::CreateMeshArrow,
          "Factory function to create an arrow mesh", "cylinder_radius"_a = 1.0,
          "cone_radius"_a = 1.5, "cylinder_height"_a = 5.0,
          "cone_height"_a = 4.0, "resolution"_a = 20, "cylinder_split"_a = 4,
          "cone_split"_a = 1);
    docstring::FunctionDocInject(
            m, "create_mesh_arrow",
            {{"cylinder_radius", "The radius of the cylinder."},
             {"cone_radius", "The radius of the cone."},
             {"cylinder_height",
              "The height of the cylinder. The cylinder is from (0, 0, 0) to "
              "(0, 0, cylinder_height)"},
             {"cone_height",
              "The height of the cone. The axis of the cone will be from (0, "
              "0, cylinder_height) to (0, 0, cylinder_height + cone_height)"},
             {"resolution",
              "The cone will be split into ``resolution`` segments."},
             {"cylinder_split",
              "The ``cylinder_height`` will be split into ``cylinder_split`` "
              "segments."},
             {"cone_split",
              "The ``cone_height`` will be split into ``cone_split`` "
              "segments."}});

    m.def("create_mesh_coordinate_frame", &geometry::CreateMeshCoordinateFrame,
          "Factory function to create a coordinate frame mesh. The coordinate "
          "frame will be centered at ``origin``. The x, y, z axis will be "
          "rendered as red, green, and blue arrows respectively.",
          "size"_a = 1.0, "origin"_a = Eigen::Vector3d(0.0, 0.0, 0.0));
    docstring::FunctionDocInject(
            m, "create_mesh_coordinate_frame",
            {{"size", "The size of the coordinate frame."},
             {"origin", "The origin of the cooridnate frame."}});

    m.def("create_mesh_moebius", &geometry::CreateMeshMoebius,
          "Factory function to create a Moebius strip.", "length_split"_a = 70,
          "width_split"_a = 15, "twists"_a = 1, "raidus"_a = 1,
          "flatness"_a = 1, "width"_a = 1, "scale"_a = 1);
    docstring::FunctionDocInject(
            m, "create_mesh_moebius",
            {{"length_split",
              "The number of segments along the Moebius strip."},
             {"width_split",
              "The number of segments along the width of the Moebius strip."},
             {"twists", "Number of twists of the Moebius strip."},
             {"radius", "The radius of the Moebius strip."},
             {"flatness", "Controls the flatness/height of the Moebius strip."},
             {"width", "Width of the Moebius strip."},
             {"scale", "Scale the complete Moebius strip."}});
}
