
int main(int argc, char **argv)
{
    // Red-Black Tree
    JRB people;
    Person *p;
    // Input parser
    IS is;
    // Makes the input parser by opening the file from argv[1]
    is = new_inputstruct(argv[1]);
    // Constructs the Red-Black tree
    people = make_jrb();
    if (fillTree(&people, is) < 0)
    {
        return -1;
    }

}
