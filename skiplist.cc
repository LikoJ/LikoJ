#include "skiplist.h"

namespace PMSkiplist {

static const int max_height = 32;   // The max height of PMSkiplist

Skiplist::SkipList(): rnd_(0xdeadbeef),
                      head_(NewNode("", "", max_height)),
                      scan_tmp_(NULL),
                      now_height_(0) {}

Node* Skiplist::NewNode(const std::string key, const std::string value, const int height) {
    size_t tmp = sizeof(Node) + sizeof(Node*) * (height - 1);
    Node *n = arena_.Allocate(tmp);
    n->key_len = key.length();
    if (n->key_len == 0) {
        n->key = NULL;
    } else {
        n->key = arena_.Allocate(n->key_len);
        memcpy(n->key, key.data(), n->key_len);
    }
    n->value_len = value.length();
    if (n->value_len == 0) {
        n->value = NULL;
    } else {
        n->value = arena_.Allocate(n->value_len);
        memcpy(n->value, value.data(), n->value_len);
    }

    n->node_height = height;
    for (int i = 0; i < n->node_height; i++) {
        n->SetNext(i, NULL);
    }

    arena_.Sync(n->key, n->ken_len);
    arena_.Sync(n->value, n->value_len);
    arena_.Sync(n, tmp);
    return n;
}

int Skiplist::RandomHeight() {
    static const unsigned int kBranching = 4;
    int height = 1;
    while (height < max_height && ((rnd_.Next() % kBranching) == 0)) {
        height++;
    }
    assert(height > 0);
    assert(height <= max_height);
    return height;
}

bool Skiplist::Write(const std::string key, const std::string value) {
    Node* prev[max_height];
    Node* x = FindGreaterOrEqual(key, prev);    // if x->key == key, update needs delete; else insert

    // Insert
    int height = RandomHeight();
    Node* n = NewNode(key, value, height);

    if (height > GetMaxHeight()) {
        for (int i = GetMaxHeight(); i < height; i++) {
            prev[i] = head_;
        }
        now_height_ = height;
    }

    for (int i = 0; i < height; i++) {
        n->NoBarrier_SetNext(i, prev[i]->NoBarrier_Next(i));
        prev[i]->SetNext(i, n);
    }

    if (x != NULL) {
        std::string keystr(x->key, x->key_length);
        if (keystr == key) {
            //Delete
        }
    }
}
}   // PMSkiplist