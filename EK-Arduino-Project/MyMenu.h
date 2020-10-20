class MyMenu
{
    public: 
    MyMenu(String caption, String what)
    {
        this->caption = caption;
        this->what = what;
    }
    String caption;
    String what;

    MyMenu * topNeighbour;
    MyMenu * bottomNeighbour;
};