#pragma once
#include "MessageQueue.h"
#include <memory>
#include <string>
#include <vector>

class Model;
class Scene;
class FrameBuffer;
class EditorManager;
class SelectionManager;
class AssetBrowser;
class HierarchyPanel;
class InspectorPanel;
class ViewportPanel;
class SceneManagerPanel;
struct GLFWwindow;

class GuiManager : public MessageQueue
{
public:
    GuiManager();
    ~GuiManager();

    void Start(GLFWwindow* aWindow);
    void Update(GLFWwindow* aWindow);
    void Close();

    void SetIcon(GLFWwindow* aWindow);
    void SetModelList(std::vector<Model*>* models);
    void SetScene(Scene* scn);
    void SetEditorManager(EditorManager* em);
    void SetFrameBuffer(FrameBuffer* fb);
    float GetViewportWidth();
    float GetViewportHeight();

protected:
    void ProcessMessage(Message* aMessage) override;

private:
    // Core components
    Scene* scene;
    EditorManager* editorManager;
    FrameBuffer* frameBuffer;
    std::vector<Model*>* modelList;

    // Shared managers
    std::unique_ptr<SelectionManager> selectionManager;
    std::unique_ptr<AssetBrowser> assetBrowser;

    // UI Panels
    std::unique_ptr<HierarchyPanel> hierarchyPanel;
    std::unique_ptr<InspectorPanel> inspectorPanel;
    std::unique_ptr<ViewportPanel> viewportPanel;
    std::unique_ptr<SceneManagerPanel> sceneManagerPanel;

    // Popup system
    bool showPopup;
    std::string popupTitle;
    std::string popupMessage;

    void InitializePanels();
    void DrawDockSpace();
    void DrawPopup();
    void ShowPopup(const std::string& title, const std::string& message);
};