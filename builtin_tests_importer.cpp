#include <cryptofuzz/builtin_tests_importer.h>
#include <cryptofuzz/repository.h>
#include <cryptofuzz/operations.h>
#include <cryptofuzz/util.h>
#include <cryptofuzz/crypto.h>
#include <stdio.h>
#include <fstream>
#include "config.h"

namespace cryptofuzz {

Builtin_tests_importer::Builtin_tests_importer(const std::string outDir) :
    outDir(outDir) {
}

void Builtin_tests_importer::ecdsa_verify_tests(void) {
    /* Test ECDSA_Verify with valid pubkey, null signature and bogus msg */
    /* Java CVE-2022-21449 */

    constexpr std::array<uint64_t, 3> digests{
        CF_DIGEST("NULL"),
        CF_DIGEST("SHA1"),
        CF_DIGEST("SHA256"),
    };

    for (size_t i = 0; i < (sizeof(repository::ECC_CurveLUT) / sizeof(repository::ECC_CurveLUT[0])); i++) {
        const uint64_t curveType = repository::ECC_CurveLUT[i].id;

        const auto x = cryptofuzz::repository::ECC_CurveToX(curveType);
        if ( x == std::nullopt ) {
            continue;
        }

        const auto y = cryptofuzz::repository::ECC_CurveToY(curveType);
        if ( y == std::nullopt ) {
            continue;
        }

        const auto bits = cryptofuzz::repository::ECC_CurveToBits(curveType);
        if ( bits == std::nullopt ) {
            continue;
        }

        for (const auto& digestType : digests) {
            nlohmann::json parameters;

            parameters["modifier"] = "";
            parameters["curveType"] = curveType;
            parameters["signature"]["pub"][0] = *x;
            parameters["signature"]["pub"][1] = *y;
            parameters["signature"]["signature"][0] = "0";
            parameters["signature"]["signature"][1] = "0";

            std::string cleartext;
            const size_t bytes = ((*bits) + 7) / 8;
            for (size_t j = 0; j < bytes; j++) {
                cleartext += std::string("ab");
            }
            parameters["cleartext"] = cleartext;

            parameters["digestType"] = digestType;

            fuzzing::datasource::Datasource dsOut2(nullptr, 0);
            cryptofuzz::operation::ECDSA_Verify op(parameters);
            op.Serialize(dsOut2);
            write(CF_OPERATION("ECDSA_Verify"), dsOut2);
        }
    }
}

void Builtin_tests_importer::Run(void) {
    {
        /* https://lists.gnupg.org/pipermail/gcrypt-devel/2022-April/005303.html */

        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("InvMod(A,B)");
        parameters["bn1"] = "18446744073709551615";
        parameters["bn2"] = "340282366762482138434845932244680310781";
        parameters["bn3"] = "";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* OpenSSL CVE-2019-1551 */

        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("ExpMod(A,B,C)");
        parameters["bn1"] = "40000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000176079519223";
        parameters["bn2"] = "8000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
        parameters["bn3"] = "13407807926845237209807376456131917626043958556151178674833163543294276330515137663421134775482798690129946803802212663956180562088664022929883876655300863";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* https://github.com/randombit/botan/issues/2424 */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["curveType"] = CF_ECC_CURVE("secp256k1");
        parameters["signature"]["pub"][0] = "55066263022277343669578718895168534326250603453777594175500187360389116729240";
        parameters["signature"]["pub"][1] = "83121579216557378445487899878180864668798711284981320763518679672151497189239";
        parameters["signature"]["signature"][0] = "110618813224107091100351766566588261013518646361399424304146461958647130377927";
        parameters["signature"]["signature"][1] = "56528019055117870811188539769759161932852696818058491284544029456598522370972";
        parameters["cleartext"] = "1111111111111111111111111111111111111111111111111111111111111111";
        parameters["digestType"] = CF_DIGEST("NULL");

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::ECDSA_Verify op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("ECDSA_Verify"), dsOut2);
    }

    {
        /* https://www.bearssl.org/gitweb/?p=BearSSL;a=commit;h=b2ec2030e40acf5e9e4cd0f2669aacb27eadb540 */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["priv"] = "11649127978725198960843318989712164899186848538742274787971553381990000200000";
        parameters["curveType"] = CF_ECC_CURVE("secp256r1");

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::ECC_PrivateToPublic op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("ECC_PrivateToPublic"), dsOut2);
    }

    {
        /* https://marc.info/?l=nettle-bugs&m=161588207403125&w=2 */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["curveType"] = CF_ECC_CURVE("secp192r1");
        parameters["signature"]["pub"][0] = "500377950244489656127360156902133231713860962293873817256";
        parameters["signature"]["pub"][1] = "2887465644744983053966665723554787622533253210326340420";
        parameters["signature"]["signature"][0] = "3657300785385429136677758856499483929823439200989267186171";
        parameters["signature"]["signature"][1] = "5795269996949228740681694823296899082100201373875130732539";
        parameters["cleartext"] = "000000000000000000000000000000000000000000000000000000000000000000000000000000";
        parameters["digestType"] = CF_DIGEST("NULL");

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::ECDSA_Verify op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("ECDSA_Verify"), dsOut2);
    }

    {
        /* https://github.com/rust-num/num-bigint/security/advisories/GHSA-v935-pqmr-g8v9 */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("Mul(A,B)");
        parameters["bn1"] = "1751908409537131537125538500170898456284834247157226603003757506211271703008740660271983629972621394589319817814743260788037132573827375525768352614087001005973802118154315411144459973145211065030321955006454014747636302404860000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000500000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000017519084095371315371255385001708984562848342471572266030037575062112717030087406602719836299726213963412282273518747979135756327447258318106026112550732285256285341873308870401245011922173311532720992339567250443769103127370206566800699963015941470784067816158765837377194325829065445567624503706135491647268992491738224567951239668590312737604150673060666707559622638573001000000000000000000000000000000000000000000000000000000000005000000000000000000000000000000000000001000032775000000000000000000000000000000000006555000000000000000000000000000000000051100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000051100000000000000000000000000000000000000000334960500000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
        parameters["bn2"] = "2797679189209576882232471079282971466667503264695828071922995641071553787114128836633873598351674085412524450747325287506043932037609877791269972006279405783477282972574002592470898217439108291870425066016929898298595568441821391851338153520189124937000565863108757585688943794492548250171920749281854240014451605847638308528814193114137174934617873035855507692132353288378935151545324567224127084761100635590318246430281864531717115971821522869960800772154013870410713514817263603701256831934923596376750827057084391388579414322761092445019958287929785367190409859424282514607161384750248744246894267400334126527666192347048382861608676859583794215545946135426985605162029699661309148361935835838049147613132815547172177034415188811380575232915936892286852306218747063936830081482936433912398871780577119329222967918013689901263150859065930010876527512050816694384495830387130178631327813690427892424009174422792234729109457869209992122111016632739872116262169954918148565743431705375512533964199931883897031598613783513129869760207318140696855650264608969668036324091587995120791606398816403750416274638670130467935818571573416233004761481486095485357525770359044716973345541886530849184417147724179886510430719371803767195605138456576869771931973189069862778685846454471080918610604506088244933811564103358698361193309937783984537245720666353471340910628393719442804519413835410769736189301882336408437075316380072193858417997872601742886028967223450825996866132982238787108062128981531579203425536924928393487168186764812389187591";
        parameters["bn3"] = "";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* https://github.com/rust-num/num-bigint/security/advisories/GHSA-v935-pqmr-g8v9 */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("Mul(A,B)");
        parameters["bn1"] = "69028706008097653771008759296007733009090437432200025025774332354052705068741683083925775765002579732651683277123414663533794063382101216667914920854067079940588879071753405904943465107319997191327289490310033427256626904851969819564163980348615183249987499158200260912636796181965630156324904015264940261382669817217063182855468061831703661150949748964560506936438414791056738678154278474357849275113970992550797225373462256934563774924633262320452003642790959047667172173004977718438718090000810281014032489334505868728062867212581466354467036853674670077958664386535446506001630130550879693857011833688104597330112388773667398881554542147364887816637152918514129094650097764846899476825554852205351307738873855";
        parameters["bn2"] = "69028706008097653771008759296007733009090437432200025025774332354052705068741683083925775765002579732651683277123414663533794063382101216667914920854067079940588879071753405904943465107319997191327289490310033427256626904851969819564163980348615183249987499158200260912636796181965630156324904015264940261382669817217063182855468061831703661150949748964560506936438414791056738678154278474357849275113970992550797225373462256934563774924633262320452003642790959047667172173004977718438718090000810281014032489334505868728062867212581466354467036853674670077958664386535446506001630130550879693857011833688104597330112388773667398881554542147364887816637152918514129094650097764846899476825554852205351307738873855";
        parameters["bn3"] = "";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* Golang CVE-2020-28362 */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("Div(A,B)");
        parameters["bn1"] = "358584519483312945644489792915316013339044213602710057025378519713093403775367136811193552946833977964852518126323811094171558766538930477294102352202754014047364502074282733465406304420098300938956566223447604825044653165378660808654220934140317386797436605647547035344251783388681175309878896312462399123638673023164006870682291688559849045866459882379320156562785922188068315473675422884081282545585551825614880625446167289218701394762051695330313268350329794670812948253161408451292594274399739014207334417010637468928423812230184276456744239174379249933197618878593848583094470518068468475726232163738251342097310131905930680849272841028310664578746119939063442765402231456939008950869322974938151228522534139372358492400367498961062887558755690449895613873241293596533751769714345671099601805085450175971895168494532362861762934814864428824254852204206617143500938338181117207880503616725299211127546512818115025162590876618636137113052173104416742502737370368561596863399736140020047330555882318083963391058248166526086306611168546208630336470221867761274649438920040568240355586672364041369629766988259672325376650178190401608867559277691476127260253790633028606347115233006648630520749773904572753178889043306410896719481572103343151257984559179126182952864654671975108029265900253271279260727300581558603833473001393454378263888146034670369648665677332387305233636144636820609542552307297712575326282483088219364797533195447769892283665149403243048734442559060379708810893453009404035139630200988165163293459771147369671507353091036915501580344363703556650202292247902549540396387945135516860171484083732331686535327533515291973835390950002074616588957299230950697588081418292226086963178976935282629084335080571441914455808103825957199068010958996277462617927335718501285237045919758104196981999105518858251126327564147190086701668929297090121373535663609818681419563075950912216202332424883068817853033496499555250307990801867571088564005161664673851694229256378115638648711967528318552466672802110330008059638067066747110516351292340621738471081023538752239974534759950969642034030534106863317181567644921956697308425095871487425542259145473032249204703959073782781409685501205242565733194139385668548679372590671175855746161005301195684973904275031298698413622895419134796753780584005400540701887874140721503247382731199473767225405047398400665892466961323377904735289306649375970695412037370248078233160600982879621588651277122758167886610061589913754157729001992275704104029729771707872079834966270939433642806206561342677692311109263950694638483652945640442464312262166385467894441744878177991237719191592195607608010968428224996795182545652439560872863894777412299579640840660951585744739521620917567421859910971325810482806189011139581218197219166242252251855118549850046046453651591153201074401132761041294786520068923893446820885001484255943824367916739172500995608700812841219561395770641417061810303000385143703697529057598332943524416497066285977920802005198594416397501518528498652367107112055866928178996003742123437324727163112165590276736110279024667065966850377340712167466237741461559888221967513797884827358475696380933541024632966766947587059450560759115100093576469861745467102088463860983605675390140416";
        parameters["bn2"] = "3116993790579457447112748375893042185883167398101478500044008951038414813546166554193604781689483905113745086116961413538364549101826535278749528984539434150825232146580662992015104392219577709281905691600684304610994639362514232471635777137096483775692374282322321416347358625601198719838192872763284693327095548585209030652989125767665945751666251666781249228208446710065016892365364036009771822392156104220019925598210296323427693657536087760841942816372079251507563423637977322478084880856956309918941946307771998189102380977934814707818456810189379347891226469007943997345786155503820059890434979939600837568201858480321503830361948946705331241965703035668559654425406269872970231035411704377372746951809528608629300945476754956955805938623351575043073752500871558991234379742909928964581409819479085944051368873563373837009101846833339984480560976752809053961770671434338978398388560204645447703724199984475770945558793845347311457128529826632524332764651151104056195862097834702989475195228699072591076643639956271557249798825124879923081240336104893239982314449426399919355106156729480992170277085428429494273862436861040694446848521568697842837821998525586760233035088260864960481620272267809178052686378651882742418710867941520497113265858736038442030674995350240215192932908040888484500094808109360042377787113853444193499624184022297844268211937472606388538142811709531473149974848425977946807173749380686205613155711460512933223334354555134039937460678031886669908081061972278020739411427328481877058901237776006124858876541489371779404732971736692599100045053044164541684533206538666952910343024817905960904473619121303855813056898229851718917041124821367390414718959907062721395423081504744782211017626663433965451804736669946279503523096053536013393926631027386543457280075815030981602365425356343807003128627861605719797723429244794283621870099180363787018874917551205103597787288212344182555953097750744619054215939539365118656189233648808439420256994775740283818416263400942546666498452648195317344939713935608853131174929552803443522193418462837588504730243153990068880453046833861795895402331014869639709627204788524884524271080365359104";
        parameters["bn3"] = "";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* SHA256 hash with 8 trailing zero bytes */
        /* Bitcoin block 125552 */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["cleartext"] = "b9d751533593ac10cdfb7b8e03cad8babc67d8eaeac0a3699b82857dacac9390";
        parameters["digestType"] = CF_DIGEST("SHA256");

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::Digest op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("Digest"), dsOut2);
    }

    {
        /* https://github.com/supranational/blst/commit/fd453524b12cc438adc65636fc52375b0f47b17e */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("InvMod(A,B)");
        parameters["bn1"] = "2277032426030151018417255337824965917622007080189340990097450826378373618725";
        parameters["bn2"] = "";
        parameters["bn3"] = "";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc_Mod_BLS12_381_R"), dsOut2);
    }

    {
        /* https://github.com/Uniswap/v3-core/pull/430/files */

        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("MulDivCeil(A,B,C)");
        parameters["bn1"] = "535006138814359";
        parameters["bn2"] = "432862656469423142931042426214547535783388063929571229938474969";
        parameters["bn3"] = "2";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* https://github.com/Uniswap/v3-core/pull/430/files */

        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("MulDivCeil(A,B,C)");
        parameters["bn1"] = "115792089237316195423570985008687907853269984659341747863450311749907997002549";
        parameters["bn2"] = "115792089237316195423570985008687907853269984659341747863450311749907997002550";
        parameters["bn3"] = "115792089237316195423570985008687907853269984653042931687443039491902864365164";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* https://github.com/golang/go/issues/13907 */

        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("ExpMod(A,B,C)");
        parameters["bn1"] = "1418360838713368667673807608798633154828274469458715950611059785834896527805417690789085638300131733678557961471105072833995051220886345238821026486295638442178801657267589731675776768680818543717651764266249120652633454343279113442582851856718699632320662426034208552957619670868361214955417792445559533666359913945094075526318633083339086293805242149773290515790029160349974149916049663724954402278771033515686640812594645899824402214411681144291057738782901589752407152546285500325727790648656476961831729737057388660643870936505482616729051729211477578353666989142857840895914122409544353738887054069413559429260476984056256590108364746161559530999482171745955856981696365351935611637940187291993977926536851499719922938174726202303844116951741128343669136727739112988600360076754991850627780768639329676710328504497510310153216648615854324605429493776944146392167255062895811127002608360939239504886537408994527412223";
        parameters["bn2"] = "1418360838713368667673807608798633154828274469458715950611059785834896527805417690789085638300131733678557961471105072833995051220886345238821026486295638442178801657267589731675776768680818543717651764266249120652633454343279113442582851856718699632320662426034208552957619670868361214955417792445559533666359913945094075526318633083339086293805242149773290515790029160349974149916049663724954402278771033515686640812594645899824402214411681144291057738782901589752407152546285500325727790648656476961831729737057388660643870936505482616729051729211477578353666989142857840895914122409544353738887054069413559429260476984056256590108364746161559530999482171745955856981696365351935611637940187291993977926536851499719922938174726202303844116951741128343669136727739112988600360076754991850627780768639329676710328504497510310153216648615854324605429493776944146392167255062895811127002608360939239504886537408994527412223";
        parameters["bn3"] = "1418360838713368667673807608798633154828274469458715950611059785834896527805417690789085638300131733678557961471105072833995051220886345238821026486295638442178801657267589731675776768680818543717651764266249120652633454343279113442582851856718699632320662426034208552957619670868361214955417792445559533666359913945094075526318633083339086293805242149773290515790029160349974149916049663724954402278771033515686640812594645899824402214411681144291057738782901589752407152546285500325727790648656476961831729737057388660643870936505482616729051729211477578353666989142857840895914122409544353738887054069413559429260476984056256590108364746161559530999482171745955856981696365351935611637940187291993977926536851499719922938174726202303844116951741128343669136727739112988600360076754991850627780768639329676710328504497510310153216648615854324605429493776944146392167255062895811127002608360939239504886537408994527412223";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* https://boringssl.googlesource.com/boringssl/+/13c9d5c69d04485a7a8840c12185c832026c8315 */

        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("ExpMod(A,B,C)");
        parameters["bn1"] = "20602718629153394716678609007360754677019714782193251481833419525142182531659985037375070145510293237451017584368188091022126870685458069386237266432";
        parameters["bn2"] = "20602718629153394716678609007360754677019714782193251481833419525142182531659985037375070145510293237451017584368188091022126870685458069386237266432";
        parameters["bn3"] = "65217794507498284936929595904182101586640070918206415302952825748181333802289344613133621268930484603009814379302693417284819444828923226393652383809";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* https://boringssl.googlesource.com/boringssl/+/801a801024febe1a33add5ddaa719e257d97aba5 */

        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("ExpMod(A,B,C)");
        parameters["bn1"] = "89884656743115795386465259539451236680898848947115328636715040578866337902750481566354238661203768010560056939935696678829394884407208311246423715319737062188883946712432742638151109800623047059726541476042502884419075341171231440736956555270413618581675255342293149119973622969239858152417678164812112068655";
        parameters["bn2"] = "89884656743115795386465259539451236680898848947115328636715040578866337902750481566354238661203768010560056939935696678829394884407208311246423715319737062188883946712432742638151109800623047059726541476042502884419075341171231440736956555270413618581675255342293149119973622969239858152417678164812112068655";
        parameters["bn3"] = "110712342659234704809244035008539225969130598408797005845753716999319814214739296549114375241509614852298035339711279727177173735185255364464495383658836380542690091228961831375338481258508265201612357782400124208382831563085709077734915006403595639818924209530864150551054958526719929068349301815032958984375";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* 512 bit inputs yielding 2**64-1 */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("ExpMod(A,B,C)");
        parameters["bn1"] = "8857680208213208509443553580592817279107514610654747966428883303770450045061809795764801319828357860402334295927798779929296899829050529494276811105535740";
        parameters["bn2"] = "6703903964971298549787012499102923063739682910296196688861780721860882015036773488400937149083451713845015929093243025426876941405973284973216824503042049";
        parameters["bn3"] = "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006083527";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* 512 bit inputs yielding 2**64+1 */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("ExpMod(A,B,C)");
        parameters["bn1"] = "10204275329685018288440986110192577709537908906243621545783045708979843711237025920291739990009664202518790138641499595280669022981609615550964238785177371";
        parameters["bn2"] = "6703903964971298549787012499102923063739682910296196688861780721860882015036773488400937149083451713845015929093243025426876941405973284973216824503042049";
        parameters["bn3"] = "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006083527";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* 512 bit inputs yielding 2**64 */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("ExpMod(A,B,C)");
        parameters["bn1"] = "7883116657589281974271912623637700107012187748714288892978317742749330820363436618231507443222736815405757624205390521540488152726116425414697649638915851";
        parameters["bn2"] = "6703903964971298549787012499102923063739682910296196688861780721860882015036773488400937149083451713845015929093243025426876941405973284973216824503042051";
        parameters["bn3"] = "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084171";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* 1024 bit inputs yielding 2**64-1 */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("ExpMod(A,B,C)");
        parameters["bn1"] = "110021198500991967651473336073512924367198080280993801155862175687208955447876903306056316802924356113531362673660885844479984811437416689853419575919456216600834216594364521611307242122099434481902932110150549263997297289055570951695734720650565352979270674449771645079830419587778176208091486492269276625052";
        parameters["bn2"] = "89884656743115795386465259539451236680898848947115328636715040578866337902750481566354238661203768010560056939935696678829394884407208311246423715319737062188883946712432742638151109800623047059726541476042502884419075341171231440736956555270413618581675255342293149119973622969239858152417678164812112068609";
        parameters["bn3"] = "179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137037";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* 1024 bit inputs yielding 2**64+1 */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("ExpMod(A,B,C)");
        parameters["bn1"] = "138410589331258564638748296036783696227294962567186300676459344541571361815053804625909677099447790672592370729455096609650139539565024626012507071092207784473803413938741074607148467420052958140679341422106167520452014781761894178531214995987524986814991021604189840198271286245691479388062392791077956993027";
        parameters["bn2"] = "89884656743115795386465259539451236680898848947115328636715040578866337902750481566354238661203768010560056939935696678829394884407208311246423715319737062188883946712432742638151109800623047059726541476042502884419075341171231440736956555270413618581675255342293149119973622969239858152417678164812112068609";
        parameters["bn3"] = "179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137037";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        /* 1024 bit inputs yielding 2**64 */
        nlohmann::json parameters;

        parameters["modifier"] = "";
        parameters["calcOp"] = CF_CALCOP("ExpMod(A,B,C)");
        parameters["bn1"] = "96140176794786042540881117365913614929332473599594543813015078247626462630063942904834778278554686465332846117127424814467831005328876422538408607223040418172986545355989261170028333879457384674630957359078099386975063368055999862340142799924547016668876836154665794866224612287640504502622973635534306776009";
        parameters["bn2"] = "89884656743115795386465259539451236680898848947115328636715040578866337902750481566354238661203768010560056939935696678829394884407208311246423715319737062188883946712432742638151109800623047059726541476042502884419075341171231440736956555270413618581675255342293149119973622969239858152417678164812112068609";
        parameters["bn3"] = "179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137859";
        parameters["bn4"] = "";

        fuzzing::datasource::Datasource dsOut2(nullptr, 0);
        cryptofuzz::operation::BignumCalc op(parameters);
        op.Serialize(dsOut2);
        write(CF_OPERATION("BignumCalc"), dsOut2);
    }

    {
        ecdsa_verify_tests();
    }
}

void Builtin_tests_importer::write(const uint64_t operation, fuzzing::datasource::Datasource& dsOut2) {
    fuzzing::datasource::Datasource dsOut(nullptr, 0);

    /* Operation ID */
    dsOut.Put<uint64_t>(operation);

    dsOut.PutData(dsOut2.GetOut());

    /* Modifier */
    dsOut.PutData(std::vector<uint8_t>(0));

    /* Module ID */
    dsOut.Put<uint64_t>(CF_MODULE("OpenSSL"));

    /* Terminator */
    dsOut.Put<bool>(false);

    {
        std::string filename = outDir + std::string("/") + util::SHA1(dsOut.GetOut());
        FILE* fp = fopen(filename.c_str(), "wb");
        fwrite(dsOut.GetOut().data(), dsOut.GetOut().size(), 1, fp);
        fclose(fp);
    }
}

} /* namespace cryptofuzz */
