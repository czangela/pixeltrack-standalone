#ifndef AlpakaDataFormats_Common_Product_h
#define AlpakaDataFormats_Common_Product_h

#include <memory>

#include "AlpakaCore/alpakaConfig.h"
#include "AlpakaCore/ProductBase.h"

namespace edm {
  template <typename T>
  class Wrapper;
}

namespace cms::alpakatools {

  namespace impl {
    template <typename TQueue>
    class ScopedContextGetterBase;
  }

  /**
     * The purpose of this class is to wrap CUDA data to edm::Event in a
     * way which forces correct use of various utilities.
     *
     * The non-default construction has to be done with ScopedContext
     * (in order to properly register the CUDA event).
     *
     * The default constructor is needed only for the ROOT dictionary generation.
     *
     * The CUDA event is in practice needed only for stream-stream
     * synchronization, but someone with long-enough lifetime has to own
     * it. Here is a somewhat natural place. If overhead is too much, we
     * can use them only where synchronization between streams is needed.
     */
  template <typename TQueue, typename T>
  class Product : public ProductBase<TQueue> {
  public:
    using Queue = TQueue;
    using Event = alpaka::Event<Queue>;

    Product() = default;  // Needed only for ROOT dictionary generation

    Product(const Product&) = delete;
    Product& operator=(const Product&) = delete;
    Product(Product&&) = default;
    Product& operator=(Product&&) = default;

  private:
    friend class impl::ScopedContextGetterBase<Queue>;
    friend class ScopedContextProduce<Queue>;
    friend class edm::Wrapper<Product<Queue, T>>;

    explicit Product(std::shared_ptr<Queue> stream, std::shared_ptr<Event> event, T data)
        : ProductBase<Queue>(std::move(stream), std::move(event)), data_(std::move(data)) {}

    template <typename... Args>
    explicit Product(std::shared_ptr<Queue> stream, std::shared_ptr<Event> event, Args&&... args)
        : ProductBase<Queue>(std::move(stream), std::move(event)), data_(std::forward<Args>(args)...) {}

    T data_;  //!
  };

}  // namespace cms::alpakatools

#endif  // AlpakaDataFormats_Common_Product_h
