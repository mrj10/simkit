#include <vector>
#include <cstdio>

template<typename PAYLOAD> class CallbackFunctor {
  public:
    virtual bool operator()(PAYLOAD p) { return true; } //This could return a status code or something else if we want
};

class BlockA;

template<typename PAYLOAD> class BlockACallbackFunctor : public CallbackFunctor<PAYLOAD> {
  public:
    BlockACallbackFunctor(BlockA *_blockAPtr) : blockAPtr(_blockAPtr) { } 
    virtual bool operator()(PAYLOAD p);
  
  private:
    BlockA *blockAPtr;
};

template<typename PAYLOAD> class DummyCallbackFunctor : public CallbackFunctor<PAYLOAD> {
  public:
    virtual bool operator()(PAYLOAD p) { printf("DUMMY CALLBACK\n"); return true; }
};

template<typename PAYLOAD> class InPort {
  public:
    InPort(CallbackFunctor<PAYLOAD> *_cf) : cf(_cf) { }
    bool receive(PAYLOAD p) { return (*cf)(p); }

  private:
    CallbackFunctor<PAYLOAD> *cf;
};

template<typename PAYLOAD> class OutPort {
  public:
    typedef InPort<PAYLOAD> sink_t;
    typedef std::vector<sink_t *> sinkvec_t;

    void connect(sink_t *sink) { sinks.push_back(sink); }
    bool send(PAYLOAD p) {
      //Returns true if any receive() on a sink returns true.  Could implement any other logic we want too.
      bool ret = false;
      for(typename sinkvec_t::iterator it = sinks.begin(), end = sinks.end(); it != end; ++it)
        if((*it)->receive(p))
          ret = true;
      return ret;
    }

  private:
    sinkvec_t sinks;
};

class BlockA {
  public:
    BlockA() {
      bacf = new BlockACallbackFunctor<int>(this);
      in = new InPort<int>(bacf);
      out = new OutPort<int>();
    }
    ~BlockA() { delete out; delete in; delete bacf; }

    bool foo(int a) { return out->send(a); }

    InPort<int> *in;
    OutPort<int> *out;
    BlockACallbackFunctor<int> *bacf;
};
