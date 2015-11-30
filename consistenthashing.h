#ifndef CONSISTENTHASHING_H
#define CONSISTENTHASHING_H

#include <stdint.h>
#include <map>
#include <list>
#include <string>
#include <utility>
#include <stdlib.h>

namespace detail {

class fnv_hash
{
public:
    uint32_t operator()(const std::string& str)
    {
        const uint32_t fnv_prime = 0x811C9DC5;
        uint32_t hash = 0;
        for(std::size_t i = 0; i < str.length(); i++)
        {
            hash *= fnv_prime;
            hash ^= str[i];
        }

        return hash;
    }
};

template<class HashFunc>
class consistent_hashing_impl
{
    struct node
    {
        node(const std::string & id, uint32_t size)
            : id_(id), size_(size) {}
        std::string id_;				//  identify
        uint32_t size_; 				//	virtual node size
    };

    struct vnode
    {
        vnode(node * owner, uint32_t hash)
            : owner_(owner), hash_(hash){}
        node *owner_;
        uint32_t hash_;
    };

    node * allocate_node(const std::string & id, uint32_t size)
    {
        return new node(id, size);
    }

    void deallocate_node(node *n)
    {
        delete n;
    }

    vnode *allocate_vnode(node * ownwer, uint32_t hash)
    {
        return new vnode(ownwer, hash);
    }

    void deallocate_vnode(vnode * n)
    {
        delete n;
    }

    void insert_node(node * n)
    {
        nodes_.push_back(n);
        for(uint32_t i = 0; i < n->size_; ++i)
        {
            char buf[100] = {0};
            itoa(i, buf, 10);
            std::string key = n->id_ + "#" + buf;
            uint32_t hash = hash_func_(key);
            vnode * new_vnode = allocate_vnode(n, hash);
            vnodes_.insert(std::pair<uint32_t, vnode*>(hash, new_vnode));
        }
    }

public:
    std::string find(const std::string & key)
    {
        if(!vnodes_.empty())
        {
            uint32_t hash = hash_func_(key);
            auto it = vnodes_.lower_bound(hash);
            if(it == vnodes_.end())
                it = vnodes_.begin();
            return it->second->owner_->id_;
        }
        return std::string("");
    }

    void insert_node(const std::string & id, uint32_t size)
    {
        node * n = allocate_node(id, size);
        if(n)
            insert_node(n);
    }

    void remove_node(const std::string & id)
    {
        if(nodes_.empty())
            return;
        auto it = nodes_.begin();
        for(; it != nodes_.end(); ++it)
        {
            if((*it)->id_ == id)
                break;
        }

        if(it != nodes_.end())
        {
            for(auto vit = vnodes_.begin(); vit != vnodes_.end(); )
            {
                if(vit->second->owner_ == *it)
                {
                    deallocate_vnode(vit->second);
                    vnodes_.erase(vit++);
                    continue;
                }
                ++vit;
            }
            deallocate_node(*it);
        }
    }
private:

    std::list<node *> nodes_;
    std::map<uint32_t, vnode *> vnodes_;
    HashFunc hash_func_;
};

} // namespcae detail

template<class HashFunc = detail::fnv_hash>
class consistent_hashing
{
    typedef std::string key_type;
    typedef std::string value_type;

public:
    consistent_hashing()
    {
        impl_ = new detail::consistent_hashing_impl<HashFunc>();
    }
    
    ~consistent_hasing()
    {
        delete impl_;
    }

    value_type find(const key_type & key)
    {
        return impl_->find(key);
    }

    void insertNode(const std::string & id, uint32_t size)
    {
        return impl_->insert_node(id, size);
    }

    void removeNode(const std::string & id)
    {
        return impl_->remove_node(id);
    }

private:
    detail::consistent_hashing_impl<HashFunc> *impl_;
};



#endif // CONSISTENTHASHING_H
