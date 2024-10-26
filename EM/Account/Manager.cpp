#include "EM/pch.h"
#include "Manager.h"
#include "Account.h"
#include "EM/ConfigManager.h"
#include "Exceptions/InvalidAccountName.h"
#include "EM/DatabaseManager.h"
#include "DBHandler/Table.h"

namespace em::account
{
    Manager* Manager::s_Instance = nullptr;

    // private
    Manager::Manager()
    {
    }

    Manager::~Manager()
    {
        delete s_Instance;
        s_Instance = nullptr;
    }

    // public static
    Manager& Manager::GetInstance()
    {
        DBG_ASSERT(s_Instance);
        return *s_Instance;
    }

    // public
    void Manager::Create()
    {
        DBG_ASSERT(s_Instance == nullptr);
        s_Instance = new Manager();
    }

    // public
    StatusCode Manager::CreateAccount(const std::string& newAccountName)
    {
        if (AccountExists(newAccountName))
        {
            printf("\nAccount Already exists: %s", newAccountName.c_str());
            return StatusCode::AccountAlreadySelected;
        }

        auto& dbMgr = DatabaseManager::GetInstance();
        auto accountTable = dbMgr.GetTable("accounts");

        db::Model model;
        model["name"] = newAccountName;
        if (!accountTable->Insert(model))
        {
            printf("\nError while creating account with name: %s", newAccountName.c_str());
            return StatusCode::GeneralFailure;
        }

        return StatusCode::Success;
    }

    // public
    StatusCode Manager::OnSwitchAccount(const std::string& newAccountName)
    {
        if (m_CurrentAccountName == newAccountName)
            return StatusCode::AccountAlreadySelected;

        if (!AccountExists(newAccountName))
            return StatusCode::AccountDoesNotExist;

        SetCurrentAccountName(newAccountName);
        return StatusCode::Success;
    }

    // public
    bool Manager::AccountExists(const std::string& accountName) const
    {
        auto accountsTable = DatabaseManager::GetInstance().GetTable("accounts");
        return accountsTable->CheckIfExists(db::Condition("name", accountName, db::Condition::Type::EQUALS));
    }

    // public
    const std::string& Manager::GetCurrentAccountName() const
    {
        return m_CurrentAccountName;
    }

    // public
    int Manager::GetCurrentAccountId() const
    {
        auto accountsTable = databaseMgr.GetTable("accounts");
        db::Model model;
        accountsTable->Select(model, db::Condition("name", m_CurrentAccountName, db::Condition::Type::EQUALS));
        return model["row_id"].asInt();
    }

    // private
    void Manager::SetCurrentAccountName(const std::string& accountName)
    {
        m_CurrentAccountName = accountName;
    }

}
