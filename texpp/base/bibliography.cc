#include "bibliography.h"


namespace texpp {
namespace base {

bool Bibliography::invoke(Parser &parser, shared_ptr<Node> node)
{
    Node::ptr bibItemNode = parser.parseBibitem();
    node->appendChild("bibliography_item", bibItemNode);
    return true;
}
bool Cite::invoke(Parser &parser, shared_ptr<Node> node)
{
    Node::ptr citeNode = parser.parseCite();
    node->appendChild("cite_item", citeNode);

    return true;
}

} // namespace base
} // namespace texpp
