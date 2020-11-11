#include <gst/gst.h>
#include <type_traits>
#include <tuple>

namespace thermo
{
    
    class GObjectWrapperBase
    {
        public:
        GObjectWrapperBase()=default;

        GObjectWrapperBase(const GObjectWrapperBase &o)noexcept
            : obj{o.obj}
        {
            if(obj){
                g_object_ref(obj);
            }
        }
        GObjectWrapperBase(GObjectWrapperBase &&)=delete;
        GObjectWrapperBase &operator=(const GObjectWrapperBase &o)noexcept {
            if(&o != this ){
                if(obj) {
                    g_object_unref(obj);
                }
                obj = o.obj;
                if(obj) {
                    g_object_ref(obj);
                }
            }
            return *this;
        }
        GObjectWrapperBase &operator=(GObjectWrapperBase &&)=delete;
        ~GObjectWrapperBase() {
            if(obj){
                g_object_unref(obj);
            }
        }
        gpointer get()const {
            return obj;
        }
        protected:
        GObjectWrapperBase(gpointer o)
            : obj{o} {
        }
        operator bool()const { return (obj != nullptr); }
        private:
            gpointer obj = nullptr;
    };
    
    template<class T > class GObjectWrapT : public GObjectWrapperBase
    {
        public:
            GObjectWrapT()=default;
            GObjectWrapT(gpointer o) :
                GObjectWrapperBase{o}
            {}
            GObjectWrapT(T *o) :
                GObjectWrapperBase{o}
            {}
            GObjectWrapT(const GObjectWrapT<T> &o)noexcept :
                GObjectWrapperBase(o) {}
            GObjectWrapT<T> &operator=(const GObjectWrapT<T> &o)noexcept
                {
                    GObjectWrapperBase::operator=(o);
                    return *this;
                }
            operator T *()const noexcept{
                return static_cast<T *>(GObjectWrapperBase::get());
            }
    };
    
    inline GObjectWrapT<GstElement> GstElementFactoryMake(const char *plugin, const char *name=nullptr)
    {
        return GObjectWrapT<GstElement>{gst_element_factory_make(plugin, name)};
    }
    
    inline GObjectWrapT<GstElement> GstPipelineNew(const char *name)
    {
        return GObjectWrapT<GstElement>{gst_pipeline_new (name)};
    }
    
    inline GObjectWrapT<GstBus> GstPipelineBus(const GObjectWrapT<GstElement> &pipeline)
    {
        return GObjectWrapT<GstBus>{gst_pipeline_get_bus(GST_PIPELINE(pipeline.get()))};
    }

    inline GObjectWrapT<GstBin> getGstBin(const GObjectWrapperBase &obj)
    {
        return GObjectWrapT<GstBin>{GST_BIN(obj.get())};
    }

    template<typename... Args>
    inline void GstBinAddMany(std::tuple<Args...> t)
    {
        std::apply(gst_bin_add_many, std::tuple_cat(t, std::make_tuple(static_cast<GstElement *>(nullptr))));
    }

    template<typename... Args, typename Element, typename... Args2>
    inline void GstBinAddMany(std::tuple<Args...> t, Element elem, Args2&&... args)
    {
        auto *ptr = static_cast<GstElement *>(elem);
        if(ptr) {
            g_object_ref(ptr);
        }
        GstBinAddMany(std::tuple_cat(t, std::make_tuple(ptr)), std::forward<Args2&&>(args)...);
    }

    template<typename... Args>
    inline void GstBinAddMany(const GObjectWrapT<GstElement> &bin, Args&& ...args)
    {
        GstBinAddMany(std::make_tuple(GST_BIN(bin.get())), std::forward<Args&&>(args)...);
    }

    template<typename Element>
    inline void GstBinAddMany(const GObjectWrapT<GstBin> &bin, const Element &elem)
    {
        auto success = gst_bin_add(GST_BIN(bin.get()), GST_ELEMENT(elem.get()));
        g_assert(success);
    }

    template<typename Element, typename... Args>
    inline void GstBinAddMany(const GObjectWrapT<GstBin> &bin, const Element &elem, Args&& ...args)
    {
        auto success = gst_bin_add(GST_BIN(bin.get()), GST_ELEMENT(elem.get()));
        g_assert(success);
        GstBinAddMany(bin, std::forward<Args&&>(args)...);
    }


    template<typename... Args>
    inline void GstElementLinkMany(std::tuple<Args...> t)
    {
        std::apply(gst_element_link_many, std::tuple_cat(t, std::make_tuple(static_cast<GstElement *>(nullptr))));
    }

    template<typename... Args, typename Element, typename... Args2>
    inline void GstElementLinkMany(std::tuple<Args...> t, Element elem, Args2&&... args)
    {
        auto *ptr = static_cast<GstElement *>(elem);
        GstElementLinkMany(std::tuple_cat(t, std::make_tuple(ptr)), std::forward<Args2&&>(args)...);
    }

    template<typename... Args>
    void GstElementLinkMany(const GObjectWrapT<GstElement> &elem, Args&& ...args)
    {
        GstElementLinkMany(std::make_tuple(static_cast<GstElement *>(elem)), std::forward<Args&&>(args)...);
    }

    
    class GstPipeline
    {
        static GstPipeline create(const std::string &name);
        GObjectWrapT<GstBus> getBus();
        operator const GObjectWrapT<::GstElement> &()const { return m_pipeline; }

        void useClock(const GObjectWrapT<::GstClock> &clock);
        void setClock(const GObjectWrapT<::GstClock> &clock);
        GObjectWrapT<::GstClock> getClock()const;
        GObjectWrapT<::GstClock> getPipelineClock()const;
        void autoClock();
        void setDelay(GstClockTime delay);
        GstClockTime getDelay()const;
        void setLatency(GstClockTime delay);
        GstClockTime getLatency()const;
    private:
        GObjectWrapT<::GstElement>  m_pipeline{};
    };

#if 0
    class GstElement
    {
        public:
        GstElement(const char *str, const char *name)
        {
            pElem = gst_element_factory_make(str, name);
            if(!pElem)
            {
                throw std::runtime_error("Failed to create gst element.");
            }
        }
        GstElement() = default;
        GstElement(const GstElement &o)noexcept
        {
            g_clear_object(&pElem);
            pElem = o.pElem;
            if(pElem)
            {
                g_object_ref (pElem);
            }
            
        }
        GstElement(GstElement && o)noexcept
        {
            pElem = o.pElem;
            o.pElem = NULL;
        }
        GstElement &operator=(GstElement o)
        {
            g_clear_object(&pElem);
            pElem = o.pElem;
            if(pElem)
            {
                g_object_ref (pElem);
            }
            return *this;            
        }
        ~GstElement()
        {
            g_clear_object(&pElem);
        }
        ::GstElement *operator->() {
            return pElem;
        }
        ::GstElement *get() {
            return pElem;
        }
        const ::GstElement *operator->()const noexcept{
            return pElem;
        }
        const ::GstElement *get()const noexcept {
            return pElem;
        }
        private:
            ::GstElement *pElem=nullptr;
    };
    
    class Pipeline
    {
    };
#endif // 0
}
