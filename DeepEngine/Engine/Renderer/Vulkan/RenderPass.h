#pragma once
#include "Factory.h"
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
    
    class RenderPass : public BaseVulkanController
    {
    protected:
        struct RenderAttachment
        {
            uint32_t ID;
            VkAttachmentDescription* Desc;
        };
        
        struct RenderSubPass
        {
            uint32_t ID;
            VkPipelineBindPoint BindPoint;
            
            std::vector<VkAttachmentReference> InputAttachments;
            std::vector<VkAttachmentReference> ColorAttachments;
            std::vector<VkAttachmentReference> ResolveAttachments;
            bool UseDepthStencilAttachment;
            VkAttachmentReference DepthStencilAttachment;
            std::vector<VkAttachmentReference> PreserveAttachments;
        };
        
        class RenderSubPassDescCreator
        {
            friend class RenderPass;
            RenderSubPassDescCreator(RenderSubPass& p_subPass, std::vector<VkSubpassDescription>& p_renderSubPassesDesc,
                std::vector<VkSubpassDependency>& p_dependencies);

        public:
            ~RenderSubPassDescCreator();

            const RenderSubPassDescCreator& AddColorAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& AddInputAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& AddResolveAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& SetDepthStencilAttachment(const RenderAttachment* p_attachment, VkImageLayout p_layout) const;
            const RenderSubPassDescCreator& AddDependency(const VkSubpassDependency& p_dependency) const;
            const RenderSubPassDescCreator& GetSubPassPtr(RenderSubPass** p_subPassPtr) const;

        private:
            RenderSubPass& _subPass;
            std::vector<VkSubpassDescription>& _renderSubPassesDesc;
            std::vector<VkSubpassDependency>& _renderSubPassesDependecies;
      };
        
    protected:
        RenderPass();
        ~RenderPass() override = default;
        
    protected:
        bool OnInitialize() final;
        void OnTerminate() override;

    public:
        VkRenderPass GetVkRenderPass() const
        { return _renderPass; }

    protected:
        void virtual Initialize() = 0;
        void virtual PostInitialize() { }
        
        void CreateRenderAttachment(const VkAttachmentDescription& p_desc, const RenderAttachment** p_attachment);
        RenderSubPassDescCreator CreateRenderSubPass(VkPipelineBindPoint p_bindPoint);

    private:
        VkRenderPass _renderPass = VK_NULL_HANDLE;
        
        std::vector<RenderAttachment> _attachments;
        std::vector<VkAttachmentDescription> _attachmentsDesc;
        std::vector<RenderSubPass> _renderSubPasses;
        std::vector<VkSubpassDescription> _renderSubPassesDesc;
        std::vector<VkSubpassDependency> _renderSubPassesDependencies;
    };

    struct RenderAttachmentHandler
    {
        uint32_t RenderPassID; // for validation only
        uint32_t AttachmentID;

        // bool operator==(const RenderAttachmentHandler&) const = default;
    };

    struct RenderSubPassHandler
    {
        uint32_t RenderPassID = 0;
        uint32_t SubPassID = 0;

        // bool operator==(const RenderSubPassHandler&) const = default;
    };

    class RenderPass2 : public VulkanObject
    {
        friend class RenderPassBuilder;
        
        struct RenderAttachment
        {
            RenderAttachmentHandler Handler;
        };
        
        struct RenderSubPass
        {
            uint32_t ID;
            VkPipelineBindPoint BindPoint;
            
            std::vector<VkAttachmentReference> InputAttachments;
            std::vector<VkAttachmentReference> ColorAttachments;
            std::vector<VkAttachmentReference> ResolveAttachments;
            bool UseDepthStencilAttachment;
            VkAttachmentReference DepthStencilAttachment;
            std::vector<VkAttachmentReference> PreserveAttachments;
        };

    public:
        RenderPass2(VkRenderPass p_handler, const std::vector<RenderAttachmentHandler>& p_attachments)
            : _handler(p_handler), _attachments(p_attachments)
        {
            
        }

        VkRenderPass GetHandler() const
        { return _handler; }

        std::vector<RenderAttachmentHandler> GetAttachments() const
        { return _attachments; }

    private:
        const VkRenderPass _handler;
        const std::vector<RenderAttachmentHandler> _attachments;
    };

    class RenderPassBuilder
    {
        template <VulkanObjectKind T>
        friend class Factory::SubFactory;
        
    protected:
        struct AttachmentDescription 
        {
            RenderAttachmentHandler Handler;
            VkAttachmentDescription Desc;
        };
        
        struct SubPassDescription
        {
            RenderSubPassHandler Handler;
            VkPipelineBindPoint BindPoint;
            
            std::vector<VkAttachmentReference> InputAttachments;
            std::vector<VkAttachmentReference> ColorAttachments;
            std::vector<VkAttachmentReference> ResolveAttachments;
            bool UseDepthStencilAttachment;
            VkAttachmentReference DepthStencilAttachment;
            std::vector<VkAttachmentReference> PreserveAttachments;
        };

        RenderPassBuilder()
        {
            _attachments.reserve(16);
            _subpasses.reserve(16);
            _dependecies.reserve(16);
        }

    public:
        virtual ~RenderPassBuilder() = default;

    protected:
        virtual void DeclareAttachments() = 0;

        RenderAttachmentHandler CreateAttachment(const VkAttachmentDescription& p_desc)
        {
            const uint32_t id = static_cast<uint32_t>(_attachments.size());
            const RenderAttachmentHandler handler { _currentRenderPassID, id };
            
            _attachments.push_back({ handler, p_desc });
            
            return handler;
        }
        
        RenderSubPassHandler CreateRenderSubPass(VkPipelineBindPoint p_bindPoint)
        {
            const uint32_t id = static_cast<uint32_t>(_subpasses.size());
            const RenderSubPassHandler handler { _currentRenderPassID, id };

            SubPassDescription subPass { handler, p_bindPoint };
            subPass.ColorAttachments.reserve(16);
            subPass.InputAttachments.reserve(16);
            subPass.ResolveAttachments.reserve(16);
            subPass.PreserveAttachments.reserve(16);
            
            _subpasses.push_back(subPass);
        
            return handler;
        }
        
        void AttachColorToSubPass(RenderSubPassHandler p_subPass, RenderAttachmentHandler p_attachment, VkImageLayout p_layout)
        {
            _subpasses[p_subPass.SubPassID].ColorAttachments.emplace_back(
                p_attachment.AttachmentID,
                p_layout
            );
        }
        
        void AttachInputToSubPass(RenderSubPassHandler p_subPass, RenderAttachmentHandler p_attachment, VkImageLayout p_layout)
        {
            _subpasses[p_subPass.SubPassID].InputAttachments.emplace_back(
                p_attachment.AttachmentID,
                p_layout
            );
        }
        
        void AttachResolveToSubPass(RenderSubPassHandler p_subPass, RenderAttachmentHandler p_attachment, VkImageLayout p_layout)
        {
            _subpasses[p_subPass.SubPassID].ResolveAttachments.emplace_back(
                p_attachment.AttachmentID,
                p_layout
            );
        }
        
        void AttachDepthStencilToSubPass(RenderSubPassHandler p_subPass, RenderAttachmentHandler p_attachment, VkImageLayout p_layout)
        {
            _subpasses[p_subPass.SubPassID].UseDepthStencilAttachment = true;
            
            _subpasses[p_subPass.SubPassID].DepthStencilAttachment = {
                p_attachment.AttachmentID,
                p_layout
            };
        }

        void AttachDependencyToSubPass(RenderSubPassHandler* p_srcSubPass, RenderSubPassHandler* p_dstSubPass,
            VkPipelineStageFlags p_srcStageMask, VkPipelineStageFlags p_dstStageMask,
            VkAccessFlags p_srcAccessFlags, VkAccessFlags p_dstAccessFlags)
        {
            // TODO: Setup access flags & stages in subpass

            _dependecies.emplace_back(
                p_srcSubPass != nullptr ? p_srcAccessFlags : VK_SUBPASS_EXTERNAL,
                p_dstSubPass->SubPassID,
                p_srcStageMask,
                p_dstStageMask,
                p_srcAccessFlags,
                p_dstAccessFlags);
        }

    private:
        uint32_t _currentRenderPassID = 0;
        std::vector<AttachmentDescription> _attachments;
        std::vector<SubPassDescription> _subpasses;
        std::vector<VkSubpassDependency> _dependecies;
    };
    
    template<>
    class Factory::SubFactory<RenderPass2>
    {
    public:
        static Ref<RenderPass2> Create(RenderPassBuilder* p_builder, Ref<VulkanObject> p_parent = nullptr)
        {
            p_builder->DeclareAttachments();

            _idCounter++;

            const std::vector<RenderPassBuilder::AttachmentDescription>& attachments = p_builder->_attachments;

            auto attachmentsDesc = std::make_unique<VkAttachmentDescription[]>(attachments.size());
            std::vector<RenderAttachmentHandler> attachmentsHandlers(attachments.size());
            for (uint32_t i = 0; i < attachments.size(); i++)
            {
                attachmentsDesc[i] = attachments[i].Desc;
                attachmentsHandlers[i] = attachments[i].Handler;
            }
            
            const std::vector<RenderPassBuilder::SubPassDescription>& subpasses = p_builder->_subpasses;
            auto subpassesDesc = std::make_unique<VkSubpassDescription[]>(subpasses.size());
            for (uint32_t i = 0; i < subpasses.size(); i++)
            {
                auto& subpass = subpasses[i];
                
                subpassesDesc[i] = VkSubpassDescription { };

                subpassesDesc[i].colorAttachmentCount = (uint32_t)subpass.ColorAttachments.size();
                if (!subpass.ColorAttachments.empty())
                {
                    subpassesDesc[i].pColorAttachments = subpass.ColorAttachments.data();
                }

                subpassesDesc[i].inputAttachmentCount = (uint32_t)subpass.InputAttachments.size();
                if (!subpass.InputAttachments.empty())
                {
                    subpassesDesc[i].pInputAttachments = subpass.InputAttachments.data();
                }

                // TODO (Kostek): assign resolve attachments
                // subpassesDesc[i].pResolveAttachments = subpass.ResolveAttachments.data();

                subpassesDesc[i].pDepthStencilAttachment = nullptr;
                
                if (subpass.UseDepthStencilAttachment)
                {
                    subpassesDesc[i].pDepthStencilAttachment = &subpass.DepthStencilAttachment;
                }

                // TODO (Kostek): assign preserve attachments
                // desc.preserveAttachmentCount = static_cast<uint32_t>(_subPass.PreserveAttachments.size());
                // desc.pPreserveAttachments = _subPass.PreserveAttachments.data();
            }
            
            VkRenderPassCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            createInfo.pAttachments = attachmentsDesc.get();
            createInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
            createInfo.pSubpasses = subpassesDesc.get();
            createInfo.dependencyCount = static_cast<uint32_t>(p_builder->_dependecies.size());
            createInfo.pDependencies = p_builder->_dependecies.data();

            VkRenderPass vulkanHandler;
            vkCreateRenderPass(_bindFactory->_vulkanInstance.GetLogicalDevice(), &createInfo, nullptr, &vulkanHandler);

            return CreateObject(new RenderPass2(vulkanHandler, attachmentsHandlers), Terminate, p_parent);
        }

    private:
        static void Terminate(VulkanObject* p_object)
        {
            vkDestroyRenderPass(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                ((RenderPass2*)p_object)->GetHandler(),
                nullptr);
        }

        static uint32_t _idCounter;
    };
    
}

