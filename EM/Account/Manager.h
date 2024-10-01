#pragma once

namespace em::account
{
    class Account;

    /**
    * This Singleton class deals with everything related to accounts
    */
    class Manager
    {
    public:
        /**
        * Creates the singleton instance.
        */
        static void Create();

        /**
        * Getter for the singleton instance.
        */
        static Manager& GetInstance();

        /**
        * This function is used to switch accounts.
        * 
        * @params [in] accountName
        *       Name of the account that we want to switch to.
        * 
        * @returns 
        *       True, if account was switched successfully.
        */
        StatusCode OnSwitchAccount(const std::string& newAccountName);

        /**
        * Returns true if the account exists with the account manager.
        * 
        * @params [in] accountName
        *       Name of the account to be checked.
        */
        bool AccountExists(const std::string& accountName) const;

        /**
        * Setter for the current account name.
        * 
        * @params [in] accountName
        *       Name of the account that will be used from now on.
        */
        void SetCurrentAccountName(const std::string& accountName);

        /**
        * Returns the account name that is currently in use.
        */
        const std::string& GetCurrentAccountName() const;

        /**
        * Destructor 
        */
        ~Manager();

    private:
        Manager();
        Manager(const Manager&) = default;

    private:
        std::string m_CurrentAccountName;
        static Manager* s_Instance;
    };
}
