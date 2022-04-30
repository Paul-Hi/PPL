#ifndef VISITOR_HPP
#define VISITOR_HPP

#include "token.hpp"
#include <fstream>
#include <unordered_map>
#include <vector>

// Counter to give classes in hierarchy a unique tag.
template <typename Base>
struct class_tag_counter
{
    static size_t s_counter;
};

template <typename Base>
size_t class_tag_counter<Base>::s_counter = 0;

// Holder class for storing the unique tag for each class in a class hierarchy.
template <typename Visitable, typename Base>
struct visitable_tag_holder
{
    static size_t s_tag;
};

template <typename Visitable, typename Base>
size_t get_tag()
{
    // const so that const Visitable and Visitable generate the same tag
    size_t& tag = visitable_tag_holder<const Visitable, const Base>::s_tag;

    if (tag == 0)
    {
        tag = ++class_tag_counter<const Base>::s_counter;
    }

    return tag;
}

// Force init on startup
template <typename Visitable, typename Base>
size_t visitable_tag_holder<Visitable, Base>::s_tag = get_tag<Visitable, Base>();

// Base class for visitables all visitables must inherit from.
template <typename Base>
struct base_visitable
{
    template <typename Visitable>
    size_t get_visitable_tag(const Visitable*) const
    {
        return get_tag<Visitable, Base>();
    }
};

// macro to define a tag member function for visitable types
#define DEFINE_VISITABLE()              \
    virtual size_t tag() const          \
    {                                   \
        return get_visitable_tag(this); \
    }

// visitable vtable - one per base class - function - combination
template <typename Base, typename Func>
class visitable_vtable
{
  public:
    template <typename Visitable>
    void add(Func f)
    {
        const size_t idx = get_tag<Visitable, Base>();

        if (idx >= table.size())
        {
            const size_t base_tag = get_tag<Base, Base>();

            Func base_function = (base_tag >= table.size() ? 0 : table[base_tag]);

            table.resize(idx + 1, base_function);
        }

        table[idx] = f;
    }

    Func operator[](size_t idx) const
    {
        if (idx >= table.size())
        {
            idx = get_tag<Base, Base>();
        }

        return table[idx];
    }

  private:
    std::vector<Func> table;
};

// base class for visitors
template <typename Base, typename ReturnType, typename ParameterType>
class base_visitor
{
  public:
    template <typename VisitorImpl, typename Visitable>
    ReturnType thunk(Base& b, ParameterType param)
    {
        VisitorImpl& real_visitor = static_cast<VisitorImpl&>(*this);

        Visitable& real_visitable = static_cast<Visitable&>(b);

        return real_visitor.visit(real_visitable, std::forward<ParameterType>(param));
    }

    using Thunk      = ReturnType (base_visitor::*)(Base&, ParameterType);
    using VTableType = visitable_vtable<const Base, Thunk>;
    using BaseType   = Base;

    ReturnType operator()(Base& b, ParameterType param)
    {
        Thunk thunk = (*vtable)[b.tag()];
        return (this->*thunk)(b, std::forward<ParameterType>(param));
    }

  private:
    template <typename VisitorImpl, typename... VisitableList>
    friend struct vtable_setter;

    const VTableType* vtable;
};

// Thunk tag used to select non-empty variadic overload
template <typename...>
struct ThunkTag
{
};

// Thunk tag used to select empty variadic overload
template <>
struct ThunkTag<>
{
};

template <typename Visitor, typename... VisitableList>
struct get_static_vtable;

// creation of vtables for given visitor and visitable types
template <typename Visitor, typename... VisitableList>
struct vtable_creation
{
    vtable_creation()
    {
        this->add_thunk<Visitor::BaseType>();

        this->add_thunks(ThunkTag<VisitableList...>());
    }

    template <typename Visitable>
    void add_thunk()
    {
        vtable.template add<Visitable>(&Visitor::template thunk<Visitor, Visitable>);
    }

    template <typename Head, typename... Tail>
    void add_thunks(ThunkTag<Head, Tail...>)
    {
        this->add_thunk<Head>();

        this->add_thunks(ThunkTag<Tail...>());
    }

    void add_thunks(ThunkTag<>){};

    friend struct get_static_vtable<Visitor, VisitableList...>;
    typename Visitor::VTableType vtable;
};

// helper struct to make vtables static -> only one vtable per base class
template <typename Visitor, typename... VisitableList>
struct get_static_vtable
{
    static vtable_creation<Visitor, VisitableList...> const s_table;

    operator const typename Visitor::VTableType *() const
    {
        return &s_table.vtable;
    }
};

template <typename Visitor, typename... VisitableList>
vtable_creation<Visitor, VisitableList...> const get_static_vtable<Visitor, VisitableList...>::s_table;

// helper struct to set vtable for given visitor and visitable list
template <typename Visitor, typename... VisitableList>
struct vtable_setter
{
    static void set_vtable(Visitor& visitor)
    {
        visitor.vtable = get_static_vtable<Visitor, VisitableList...>();
    }
};

#define DEFINE_VISITOR_AND_VISITABLES(VisitorImpl, ...) vtable_setter<VisitorImpl, __VA_ARGS__>::set_vtable(*this)

#endif VISITOR_HPP