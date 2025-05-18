#include "components/dialog.h"

#include <ImGuiFileDialog.h>
#include <plog/Log.h>

#include <string>
#include <utility>
#include <vector>

#include "assets/icons.h"

namespace ui::dialog {

  void Handler::add_dialog(callback_t         callback,
                           const std::string& title,
                           const std::string& prompt,
                           const std::string& path,
                           const char*        filter) {
    IGFD::FileDialogConfig config;
    config.path = path;

    auto dialog = std::make_unique<IGFD::FileDialog>();
    dialog->OpenDialog(title, prompt, filter, config);
    dialog->SetFileStyle(IGFD_FileStyleByTypeDir,
                         "",
                         ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                         ICON_FA_FOLDER);

    m_dialogs.emplace_back(std::move(dialog), std::move(callback));
    PLOGD << "Opening a dialog " << title << " in " << path;
    PLOGD << "Total dialogs: " << m_dialogs.size();
  }

  void Handler::handle() {
    auto it = m_dialogs.begin();
    while (it != m_dialogs.end()) {
      auto& [dialog, callback] = *it;
      const auto key           = dialog->GetOpenedKey();
      if (dialog->Display(key)) {
        if (dialog->IsOk()) {
          PLOGD << "Callback for dialog " << key;
          callback(dialog.get());
        } else {
          PLOGD << "Dialog " << key << " is closed";
        }
        dialog->Close();

        it = m_dialogs.erase(it);
        PLOGD << "Dialog " << key
              << " removed from handler. Remaining: " << m_dialogs.size();
      } else {
        ++it;
      }
    }
  }

} // namespace ui::dialog
