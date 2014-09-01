#include "texpack.hpp"
//#include "treetree/tree.hpp"

namespace graphics
{
	namespace
	{
		struct tex_node
		{
			tex_node() : image_id(nullptr) { child[0] = child[1] = nullptr; }
			tex_node* child[2];
			rect rc;
			surface_ptr image_id;
			tex_node* insert(surface_ptr id, const rect& r);
			void blit(surface_ptr dest);
			~tex_node();
		};

		tex_node* tex_node::insert(surface_ptr id, const rect& r)
		{
			if(child[0] != nullptr && child[1] != nullptr) {
				// not a leaf node
				auto nnode = child[0]->insert(id, r);
				if(nnode != nullptr) {
					return nnode;
				}
				return child[1]->insert(id, r);
			}
			if(image_id != nullptr) {
				return nullptr;
			}
			if(r.w() > rc.w() || r.h() > rc.h()) {
				return nullptr;
			}
			if(rc.w() == r.w() && rc.h() == r.h()) {
				image_id = id;
				return this;
			}
			child[0] = new tex_node();
			child[1] = new tex_node();
			int dw = rc.w() - r.w();
			int dh = rc.h() - r.h();
			if(dw > dh) {
				child[0]->rc = rect::from_coordinates(rc.x(), rc.y(), rc.x()+r.w()-1, rc.y2()-1);
				child[1]->rc = rect::from_coordinates(rc.x()+r.w(), rc.y(), rc.x2()-1, rc.y2()-1);
			} else {
				child[0]->rc = rect::from_coordinates(rc.x(), rc.y(), rc.x2()-1, rc.y()+r.h()-1);
				child[1]->rc = rect::from_coordinates(rc.x(), rc.y()+r.h(), rc.x2()-1, rc.y2()-1);
			}
			return child[0]->insert(id, r);
		}

		void tex_node::blit(surface_ptr dest)
		{
			// in-order traversal
			if(child[0] != nullptr) {
				child[0]->blit(dest);
			}
			if(image_id != nullptr) {
				dest->blit_clipped(image_id, rc);
			}
			if(child[1] != nullptr) {
				child[1]->blit(dest);
			}
		}

		tex_node::~tex_node()
		{
			if(child[0] != nullptr) {
				delete child[0];
			}
			if(child[1] != nullptr) {
				delete child[1];
			}
		}
	}

	packer::packer(const std::vector<surface_ptr> inp, int max_length)
	{
		// Create a list of rectangles based on the size of each surface and sort by perimeter.
		typedef std::vector<std::pair<surface_ptr,rect>> rlist;
		rlist rect_list;
		for(const auto& p : inp) {
			rect_list.emplace_back(std::make_pair(p, rect(0, 0, p->width(), p->height())));
		}
		std::sort(rect_list.begin(), rect_list.end(), [](const std::pair<surface_ptr,rect>& lhs, const std::pair<surface_ptr,rect>& rhs) {
				return lhs.second.perimeter() > rhs.second.perimeter();
		});

		std::vector<tex_node*> root;
		root.emplace_back(new tex_node());
		root.front()->rc = rect(0, 0, max_length, max_length);
		for(auto& r : rect_list) {
			for(auto& tn : root) {
				root.back()->insert(r.first, r.second);
			}
		}

		for(auto& tn : root) {
			surface_ptr dest = std::make_shared<graphics::surface>(max_length, max_length);
			tn->blit(dest);
			outp.emplace_back(texture_pack(graphics::texture(dest, graphics::TextureFlags::NONE), nullptr));
			dest->save("images/temp/nn.png");
		}

		for(auto& tn : root) {
			delete tn;
		}
	}
}
