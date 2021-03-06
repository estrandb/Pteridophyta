
#include "CompWindowLobby.h"
#include "ContextClient.h"


namespace bali{

CompWindowLobby::CompWindowLobby()
{
}

CompWindowLobby::~CompWindowLobby()
{
}

uint32_t CompWindowLobby::initialize(Context& cc)
{

    cc.net.startClient(5676,sf::IpAddress("192.168.1.13"));
    //sf::sleep(sf::seconds(6));
    std::cout << "CompWindowLobby::initialize()" << ", "
              << cc.mp.player.name << ", "
              << cc.mp.player.pass << ", "
              << cc.mp.player.server_port << ", "
              << cc.mp.player.server_ip << std::endl;


    //Create Main Vertical Layout box
    boxMainVert =  sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 5.0f );

    //Create Team Label, add to main box
    sfg::Box::Ptr row = sfg::Box::Create( sfg::Box::Orientation::HORIZONTAL, 5.0f );
    row->Pack(sfg::Label::Create("The Team"));
    boxMainVert->Pack(row);

    //Create Name List Vertical Layout box as placeholder
    boxNames = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 10.0f );
    boxMainVert->Pack(boxNames);

    //Create Join Button Horiz Layout box, add to main box.
    //Add button, and connect button to function
    row = sfg::Box::Create( sfg::Box::Orientation::HORIZONTAL, 5.0f );
    joinTeam1Button = sfg::Button::Create("Join The Team");
    joinTeam1Button->GetSignal( sfg::Widget::OnLeftClick ).Connect( std::bind(&CompWindowLobby::doJoinTeam1, this, (ContextClient*)&cc) );
    row->Pack(joinTeam1Button);
    boxMainVert->Pack(row);

    //Create Ready button Horiz layout box, add to main box
    //Add button, and connect button to function
    row = sfg::Box::Create( sfg::Box::Orientation::HORIZONTAL, 10.0f );
    readyButton = sfg::Button::Create("Ready");
    readyButton->GetSignal( sfg::Widget::OnLeftClick ).Connect( std::bind(&CompWindowLobby::doReady, this, (ContextClient*)&cc) );
    readyButton->Show(false);//Don't show until team is joined.
    row->Pack(readyButton);
    boxMainVert->Pack(row);

    //Create Spinner & Wait-Message Horiz layout box, add to main box
    //Add spinner, and wait message
    row = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 10.0f);
    spinner = sfg::Spinner::Create();
    spinner->Show(false);
    msg = sfg::Label::Create("Waiting for Host to start...");
    msg->Show(false);
    row->Pack(spinner);
    row->Pack(msg);
    boxMainVert->Pack(row);

    //Create Window
    //Add main box to window
    window = sfg::Window::Create();
    window->SetTitle("Lobby");
    window->SetPosition(sf::Vector2f(100.0f,100.0f));
    window->SetRequisition(sf::Vector2f(200,200));
    window->Add(boxMainVert);

    //Add window to desktop
    desk.Add(window);

    return 0;
}
uint32_t CompWindowLobby::doWindowEvent(Context& cc, sf::Event & event)
{
    desk.HandleEvent(event);
    return 0;
}

uint32_t CompWindowLobby::doLocalInputs(Context& cc)
{
    return 0;
}
uint32_t CompWindowLobby::doUpdate(Context& cc)
{
    desk.Update(deskUpdateClock.restart().asSeconds());
    return 0;
}

uint32_t CompWindowLobby::doDraw(Context& c)
{
    ContextClient& cc = *((ContextClient*)&c);
    cc.sfGui.Display(cc.window);
    return 0;
}

uint32_t CompWindowLobby::cleanup(Context& cc)
{
    return 0;
}

void CompWindowLobby::doReady(ContextClient* cc)
{
    cc->mp.player.setReady();
    cc->mp.player.stateClient = StatePlayerClient::SendReady;
    readyButton->Show(false);
    ready=1;
}

uint32_t CompWindowLobby::isReady(){
    return ready;
}

void CompWindowLobby::clearNames()
{
    boxNames->RemoveAll();
}

void CompWindowLobby::addName(std::string name)
{
    sfg::Box::Ptr row = sfg::Box::Create( sfg::Box::Orientation::HORIZONTAL, 5.0f );
    row->Pack(sfg::Entry::Create(name),true,true);
    boxNames->Pack(row);
}


void CompWindowLobby::gotIdAck(ContextClient& cc, std::string mapName)
{
    cc.mapName = mapName;
    readyButton->Show(true);
    cc.mp.player.setIdentity();
}

void CompWindowLobby::gotIdNack(ContextClient& cc)
{
    joinTeam1Button->Show(true);
    spinner->Show(false);
    spinner->Stop();
    msg->Show(false);
}

void CompWindowLobby::doJoinTeam1(ContextClient* cc)
{
    //Send Id - declare team 1
    cc->mp.player.team = 1;
    cc->mp.player.stateClient = StatePlayerClient::SendId;

    joinTeam1Button->Show(false);

    spinner->Show(true);
    spinner->Start();
    msg->Show(true);
}


}//end namespace bali
