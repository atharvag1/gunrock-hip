#pragma once

#include <cassert>
#include <tuple>

#include <gunrock/util/type_traits.hxx>

#include <gunrock/formats/formats.hxx>

#include <gunrock/graph/properties.hxx>
#include <gunrock/graph/vertex_pair.hxx>

namespace gunrock {
namespace graph {
namespace detail {

using namespace format;

template <typename vertex_t, typename edge_t, typename weight_t>
class graph_base_t {

    using vertex_type = vertex_t;
    using edge_type   = edge_t;
    using weight_type = weight_t;

    using vertex_pair_type = vertex_pair_t<vertex_t>;
    using properties_type = graph_properties_t;

    using graph_base_type = graph_base_t<vertex_type, edge_type, weight_type>;

    public:
        graph_base_t() : 
            _number_of_vertices(0), 
            _number_of_edges(0), 
            _properties() {}

        graph_base_t(vertex_type number_of_vertices, edge_type number_of_edges) :
            _number_of_vertices(number_of_vertices),
            _number_of_edges(number_of_edges),
            _properties() {}

        graph_base_t(vertex_type number_of_vertices, edge_type number_of_edges, properties_type properties) :
            _number_of_vertices(number_of_vertices),
            _number_of_edges(number_of_edges),
            _properties(properties) {}

        vertex_type get_number_of_vertices() { return _number_of_vertices; }
        edge_type get_number_of_edges() { return _number_of_edges; }
        bool is_directed() { return _properties.directed; }

        // Pure Virtual Functions:: must be implemented in derived classes
        __host__ __device__ __forceinline__
        virtual edge_type get_neighbor_list_length(const vertex_type& v) const = 0;
        
        __host__ __device__ __forceinline__
        virtual vertex_type get_source_vertex(const edge_type& e) const = 0;
        
        // __host__ __device__ __forceinline__
        // virtual vertex_type get_destination_vertex(const edge_type& e) const = 0;
        
        // __host__ __device__ __forceinline__
        // virtual vertex_pair_type get_source_and_destination_vertices(const edge_type& e) const = 0; // XXX: return type?
        
        // __host__ __device__ __forceinline__
        // virtual edge_type get_edge(const vertex_type& source, const vertex_type& destination) const = 0;

    protected:
        vertex_type     _number_of_vertices;
        edge_type       _number_of_edges;
        properties_type _properties;


}; // struct graph_base_t

}   // namespace detail
}   // namespace graph
}   // namespace gunrock