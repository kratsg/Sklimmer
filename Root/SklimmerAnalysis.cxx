#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <Sklimmer/SklimmerAnalysis.h>

#include <TLorentzVector.h>
#include <TVector3.h>

#include <cstdlib>
#include <string>


//Still really need to implement a systematics framework

// this is needed to distribute the algorithm to the workers
ClassImp(SklimmerAnalysis)



SklimmerAnalysis :: SklimmerAnalysis()
{
	// Here you put any code for the base initialization of variables,
	// e.g. initialize all pointers to 0.  Note that you should only put
	// the most basic initialization here, since this method will be
	// called on both the submission and the worker node.  Most of your
	// initialization code will go into histInitialize() and
	// initialize().

}



EL::StatusCode SklimmerAnalysis :: setupJob (EL::Job& job)
{
	// Here you put code that sets up the job on the submission object
	// so that it is ready to work with your algorithm, e.g. you can
	// request the D3PDReader service or add output files.  Any code you
	// put here could instead also go into the submission script.  The
	// sole advantage of putting it here is that it gets automatically
	// activated/deactivated when you add/remove the algorithm from your
	// job, which may or may not be of value to you.

	job.useD3PDReader ();

	return EL::StatusCode::SUCCESS;
}



EL::StatusCode SklimmerAnalysis :: histInitialize ()
{
	// Here you do everything that needs to be done at the very
	// beginning on each worker node, e.g. create histograms and output
	// trees.  This method gets called before any input files are
	// connected.

	h_nevents = new TH1F("h_nevents", "h_nevents", 10, 0, 10);
	wk()->addOutput (h_nevents);



	return EL::StatusCode::SUCCESS;
}



EL::StatusCode SklimmerAnalysis :: fileExecute ()
{
	// Here you do everything that needs to be done exactly once for every
	// single file, e.g. collect a list of all lumi-blocks processed
	return EL::StatusCode::SUCCESS;
}



EL::StatusCode SklimmerAnalysis :: changeInput (bool firstFile)
{
	// Here you do everything you need to do when we change input files,
	// e.g. resetting branch addresses on trees.  If you are using
	// D3PDReader or a similar service this method is not needed.
	return EL::StatusCode::SUCCESS;
}



EL::StatusCode SklimmerAnalysis :: initialize ()
{
	// Here you do everything that you need to do after the first input
	// file has been connected and before the first event is processed,
	// e.g. create additional histograms based on which variables are
	// available in the input files.  You can also create all of your
	// histograms and trees in here, but be aware that this method
	// doesn't get called if no events are processed.  So any objects
	// you create here won't be available in the output if you have no
	// input events.

	std::cout << Form("Copy. This is: %s",this->GetName()) << std::endl;
	whichsyst =  SystErr::NONE;
	TString tmpname(this->GetName() );
	if(tmpname.Contains("_Syst_")){
		tmpname.Remove(0,22);
		whichsyst = static_cast<SystErr::Syste>(tmpname.Atoi()) ;
		std::cout << Form("whichsyst is now: %d",whichsyst) << std::endl; 
	}




	if (!RJTool){
		throw std::string ("No RJTool configured");
	}



	std::cout << std::getenv("ROOTCOREBIN") << std::endl;

	string rootcorebinpath(std::getenv("ROOTCOREBIN") ) ;

	isData = false;
	isAtlfast = false;
	isMC12b = true;
	useLeptonTrigger = true;


	

	m_susy_obj->initialize(isData, isAtlfast, isMC12b, useLeptonTrigger);
	m_susy_obj->SetJetCalib(true);

	//bool useJVF = true;

	// BTagCalibTool = new ROOT::BTagCalib("MV1",\
	// 	rootcorebinpath + "/../SUSYTools/data/BTagCalibration.env",\
	// 	rootcorebinpath + "/../SUSYTools/data/",\
	// 	"0_7892",\
	// 	useJVF,\
	// 	0.7892) ;


	RJTool->initialize( rootcorebinpath + "/data/RJigsaw/RJigsawConfig/hemisphere1",
						rootcorebinpath + "/data/RJigsaw/RJigsawConfig/hemisphere2");
	// RJTool->initialize("./RJigsawConfig/hemisphere1","./RJigsawConfig/hemisphere2");
	RJTool->resetHists();


	event = wk()->d3pdreader();
	output = EL::getNTupleSvc (wk(), "output", Form("susy_%d",whichsyst ) );
	std::cout << "Tree Name here is: " << output->treeName() << std::endl;
	std::cout << "Output Name here is: " << output->GetName() << std::endl;


	//-- connect to the input TTree
	TTree *inputTree = wk()->tree(); // this is the input tree


	std::cout << "Connected to tree"  << std::endl;




	if (output){


	output->tree()->Branch( "RJVars_sHatR_0_0_0",                  &RJVars_sHatR_0_0_0,   "RJVars_sHatR_0_0_0/F");   
	output->tree()->Branch( "RJVars_E12_0_0_0",                    &RJVars_E12_0_0_0,   "RJVars_E12_0_0_0/F");   
	output->tree()->Branch( "RJVars_gamma_R_0_0_0",                &RJVars_gamma_R_0_0_0,   "RJVars_gamma_R_0_0_0/F");   
	output->tree()->Branch( "RJVars_dphi_Beta_R_0_0_0",            &RJVars_dphi_Beta_R_0_0_0,   "RJVars_dphi_Beta_R_0_0_0/F");   
	output->tree()->Branch( "RJVars_dphi_leg1_leg2_0_0_0",         &RJVars_dphi_leg1_leg2_0_0_0,   "RJVars_dphi_leg1_leg2_0_0_0/F");   
	output->tree()->Branch( "RJVars_gamma_Rp1_0_0_0",              &RJVars_gamma_Rp1_0_0_0,   "RJVars_gamma_Rp1_0_0_0/F");   
	output->tree()->Branch( "RJVars_dphi_Beta_Rp1_Beta_R_0_0_0",   &RJVars_dphi_Beta_Rp1_Beta_R_0_0_0,   "RJVars_dphi_Beta_Rp1_Beta_R_0_0_0/F");   
	output->tree()->Branch( "RJVars_MDeltaR_0_0_0",                &RJVars_MDeltaR_0_0_0,   "RJVars_MDeltaR_0_0_0/F");   
	output->tree()->Branch( "RJVars_Eleg1_0_0_0",                  &RJVars_Eleg1_0_0_0,   "RJVars_Eleg1_0_0_0/F");   
	output->tree()->Branch( "RJVars_Eleg2_0_0_0",                  &RJVars_Eleg2_0_0_0,   "RJVars_Eleg2_0_0_0/F");   
	output->tree()->Branch( "RJVars_costhetaRp1_0_0_0",            &RJVars_costhetaRp1_0_0_0,   "RJVars_costhetaRp1_0_0_0/F");   
	output->tree()->Branch( "RJVars_costhetaR_0_0_0",            &RJVars_costhetaR_0_0_0,   "RJVars_costhetaR_0_0_0/F");   


		// output->tree()->Branch( "RJVars_sqrtsHat_0_0_0", &RJVars_sqrtsHat_0_0_0, "RJVars_sqrtsHat_0_0_0/F");               
		// output->tree()->Branch( "RJVars_invgamma_0_0_0", &RJVars_invgamma_0_0_0, "RJVars_invgamma_0_0_0/F");               
		// output->tree()->Branch( "RJVars_mdelta_0_0_0", &RJVars_mdelta_0_0_0, "RJVars_mdelta_0_0_0/F");               
		// output->tree()->Branch( "RJVars_cospt_0_0_0", &RJVars_cospt_0_0_0, "RJVars_cospt_0_0_0/F");               

		// output->tree()->Branch( "RJVars_M_0_0_0", &RJVars_M_0_0_0, "RJVars_M_0_0_0/F");               
		// output->tree()->Branch( "RJVars_M_0_0_1", &RJVars_M_0_0_1             , "RJVars_M_0_0_1/F");               
		// output->tree()->Branch( "RJVars_M_1_0_0", &RJVars_M_1_0_0             , "RJVars_M_1_0_0/F");               
		// output->tree()->Branch( "RJVars_M_1_0_1", &RJVars_M_1_0_1             , "RJVars_M_1_0_1/F");               
		// output->tree()->Branch( "RJVars_M_1_1_0", &RJVars_M_1_1_0             , "RJVars_M_1_1_0/F");               
		// output->tree()->Branch( "RJVars_M_1_1_1", &RJVars_M_1_1_1             , "RJVars_M_1_1_1/F");               
		// output->tree()->Branch( "RJVars_M_2_0_0", &RJVars_M_2_0_0             , "RJVars_M_2_0_0/F");               
		// output->tree()->Branch( "RJVars_M_2_0_1", &RJVars_M_2_0_1             , "RJVars_M_2_0_1/F");               
		//output->tree()->Branch( "RJVars_M_2_1_0", &RJVars_M_2_1_0             , "RJVars_M_2_1_0/F");               
		// output->tree()->Branch( "RJVars_M_2_1_1", &RJVars_M_2_1_1             , "RJVars_M_2_1_1/F");     
		//output->tree()->Branch( "RJVars_MDecay_1_1_0", &RJVars_MDecay_1_1_0             , "RJVars_MDecay_1_1_0/F");               
		// output->tree()->Branch( "RJVars_MDecay_1_1_1", &RJVars_MDecay_1_1_1             , "RJVars_MDecay_1_1_1/F");  
		//output->tree()->Branch( "RJVars_MDecay_2_1_0", &RJVars_MDecay_2_1_0             , "RJVars_MDecay_2_1_0/F");               
		// output->tree()->Branch( "RJVars_MDecay_2_1_1", &RJVars_MDecay_2_1_1             , "RJVars_MDecay_2_1_1/F");  
		// output->tree()->Branch( "RJVars_dPhi_0_0_0", &RJVars_dPhi_0_0_0          , "RJVars_dPhi_0_0_0/F");                  
		// output->tree()->Branch( "RJVars_dPhi_0_0_1", &RJVars_dPhi_0_0_1          , "RJVars_dPhi_0_0_1/F");                  
		// output->tree()->Branch( "RJVars_dPhi_1_0_0", &RJVars_dPhi_1_0_0          , "RJVars_dPhi_1_0_0/F");                  
		// output->tree()->Branch( "RJVars_dPhi_1_0_1", &RJVars_dPhi_1_0_1          , "RJVars_dPhi_1_0_1/F");                  
		// output->tree()->Branch( "RJVars_dPhi_2_0_0", &RJVars_dPhi_2_0_0          , "RJVars_dPhi_2_0_0/F");                  
		// output->tree()->Branch( "RJVars_dPhi_2_0_1", &RJVars_dPhi_2_0_1,           "RJVars_dPhi_2_0_1/F");                   
		//output->tree()->Branch( "RJVars_dPhi_1_1_0", &RJVars_dPhi_1_1_0          , "RJVars_dPhi_1_1_0/F");                  
		// output->tree()->Branch( "RJVars_dPhi_1_1_1", &RJVars_dPhi_1_1_1          , "RJVars_dPhi_1_1_1/F");                  
		//output->tree()->Branch( "RJVars_dPhi_2_1_0", &RJVars_dPhi_2_1_0          , "RJVars_dPhi_2_1_0/F");                  
		// output->tree()->Branch( "RJVars_dPhi_2_1_1", &RJVars_dPhi_2_1_1,           "RJVars_dPhi_2_1_1/F");       
		// output->tree()->Branch( "RJVars_dPhiVis_0_0_0", &RJVars_dPhiVis_0_0_0       , "RJVars_dPhiVis_0_0_0/F");                     
		// output->tree()->Branch( "RJVars_dPhiVis_0_0_1", &RJVars_dPhiVis_0_0_1       , "RJVars_dPhiVis_0_0_1/F");                     
		// output->tree()->Branch( "RJVars_dPhiVis_1_0_0", &RJVars_dPhiVis_1_0_0       , "RJVars_dPhiVis_1_0_0/F");                     
		// output->tree()->Branch( "RJVars_dPhiVis_1_0_1", &RJVars_dPhiVis_1_0_1       , "RJVars_dPhiVis_1_0_1/F");                     
		// output->tree()->Branch( "RJVars_dPhiVis_2_0_0", &RJVars_dPhiVis_2_0_0       , "RJVars_dPhiVis_2_0_0/F");                     
		// output->tree()->Branch( "RJVars_dPhiVis_2_0_1", &RJVars_dPhiVis_2_0_1,        "RJVars_dPhiVis_2_0_1/F");                         
		//output->tree()->Branch( "RJVars_dPhiVis_1_1_0", &RJVars_dPhiVis_1_1_0       , "RJVars_dPhiVis_1_1_0/F");                     
		// output->tree()->Branch( "RJVars_dPhiVis_1_1_1", &RJVars_dPhiVis_1_1_1       , "RJVars_dPhiVis_1_1_1/F");                     
		//output->tree()->Branch( "RJVars_dPhiVis_2_1_0", &RJVars_dPhiVis_2_1_0       , "RJVars_dPhiVis_2_1_0/F");                     
		// output->tree()->Branch( "RJVars_dPhiVis_2_1_1", &RJVars_dPhiVis_2_1_1,        "RJVars_dPhiVis_2_1_1/F");              
		// output->tree()->Branch( "RJVars_cosTheta_0_0_0", &RJVars_cosTheta_0_0_0      , "RJVars_cosTheta_0_0_0/F");                      
		// output->tree()->Branch( "RJVars_cosTheta_0_0_1", &RJVars_cosTheta_0_0_1      , "RJVars_cosTheta_0_0_1/F");                      
		// output->tree()->Branch( "RJVars_cosTheta_1_0_0", &RJVars_cosTheta_1_0_0      , "RJVars_cosTheta_1_0_0/F");                      
		// output->tree()->Branch( "RJVars_cosTheta_1_0_1", &RJVars_cosTheta_1_0_1      , "RJVars_cosTheta_1_0_1/F");                      
		// output->tree()->Branch( "RJVars_cosTheta_2_0_0", &RJVars_cosTheta_2_0_0      , "RJVars_cosTheta_2_0_0/F");                      
		// output->tree()->Branch( "RJVars_cosTheta_2_0_1", &RJVars_cosTheta_2_0_1      , "RJVars_cosTheta_2_0_1/F");                       
		//output->tree()->Branch( "RJVars_cosTheta_1_1_0", &RJVars_cosTheta_1_1_0      , "RJVars_cosTheta_1_1_0/F");                      
		// output->tree()->Branch( "RJVars_cosTheta_1_1_1", &RJVars_cosTheta_1_1_1      , "RJVars_cosTheta_1_1_1/F");                      
		//output->tree()->Branch( "RJVars_cosTheta_2_1_0", &RJVars_cosTheta_2_1_0      , "RJVars_cosTheta_2_1_0/F");                      
		// output->tree()->Branch( "RJVars_cosTheta_2_1_1", &RJVars_cosTheta_2_1_1      , "RJVars_cosTheta_2_1_1/F");                      
		// output->tree()->Branch( "RJVars_dPhiDecay_0_0_0", &RJVars_dPhiDecay_0_0_0     , "RJVars_dPhiDecay_0_0_0/F");                       
		// output->tree()->Branch( "RJVars_dPhiDecay_0_0_1", &RJVars_dPhiDecay_0_0_1     , "RJVars_dPhiDecay_0_0_1/F");                       
		// output->tree()->Branch( "RJVars_dPhiDecay_1_0_0", &RJVars_dPhiDecay_1_0_0     , "RJVars_dPhiDecay_1_0_0/F");                       
		// output->tree()->Branch( "RJVars_dPhiDecay_1_0_1", &RJVars_dPhiDecay_1_0_1     , "RJVars_dPhiDecay_1_0_1/F");                       
		// output->tree()->Branch( "RJVars_dPhiDecay_2_0_0", &RJVars_dPhiDecay_2_0_0     , "RJVars_dPhiDecay_2_0_0/F");                       
		// output->tree()->Branch( "RJVars_dPhiDecay_2_0_1", &RJVars_dPhiDecay_2_0_1     , "RJVars_dPhiDecay_2_0_1/F");                       
		// output->tree()->Branch( "RJVars_cosThetaDecay_0_0_0", &RJVars_cosThetaDecay_0_0_0 , "RJVars_cosThetaDecay_0_0_0 /F");                           
		// output->tree()->Branch( "RJVars_cosThetaDecay_0_0_1", &RJVars_cosThetaDecay_0_0_1 , "RJVars_cosThetaDecay_0_0_1 /F");                           
		// output->tree()->Branch( "RJVars_cosThetaDecay_1_0_0", &RJVars_cosThetaDecay_1_0_0 , "RJVars_cosThetaDecay_1_0_0 /F");                           
		// output->tree()->Branch( "RJVars_cosThetaDecay_1_0_1", &RJVars_cosThetaDecay_1_0_1 , "RJVars_cosThetaDecay_1_0_1 /F");                           
		// output->tree()->Branch( "RJVars_cosThetaDecay_2_0_0", &RJVars_cosThetaDecay_2_0_0 , "RJVars_cosThetaDecay_2_0_0 /F");                           
		// output->tree()->Branch( "RJVars_cosThetaDecay_2_0_1", &RJVars_cosThetaDecay_2_0_1 , "RJVars_cosThetaDecay_2_0_1 /F");                           
		// output->tree()->Branch( "RJVars_gamma_0_0_0", &RJVars_gamma_0_0_0         , "RJVars_gamma_0_0_0/F");                   
		// output->tree()->Branch( "RJVars_gamma_0_0_1", &RJVars_gamma_0_0_1         , "RJVars_gamma_0_0_1/F");                   


		// output->tree()->Branch( "RJVars_M_Tot_Mean" , &RJVars_M_Tot_Mean,"RJVars_M_Tot_Mean/F");            
		// output->tree()->Branch( "RJVars_M_0_Mean" , &RJVars_M_0_Mean,"RJVars_M_0_Mean/F");          
		//output->tree()->Branch( "RJVars_M_1_Mean" , &RJVars_M_1_Mean,"RJVars_M_1_Mean/F");          
		// output->tree()->Branch( "RJVars_dPhi_Tot_Mean" , &RJVars_dPhi_Tot_Mean,"RJVars_dPhi_Tot_Mean/F");               
		// output->tree()->Branch( "RJVars_dPhi_0_Mean" , &RJVars_dPhi_0_Mean,"RJVars_dPhi_0_Mean/F");             
		//output->tree()->Branch( "RJVars_dPhi_1_Mean" , &RJVars_dPhi_1_Mean,"RJVars_dPhi_1_Mean/F");             
		// output->tree()->Branch( "RJVars_dPhiVis_Tot_Mean" , &RJVars_dPhiVis_Tot_Mean  ,"RJVars_dPhiVis_Tot_Mean/F");                  
		// output->tree()->Branch( "RJVars_dPhiVis_0_Mean" , &RJVars_dPhiVis_0_Mean,"RJVars_dPhiVis_0_Mean/F");                
		//output->tree()->Branch( "RJVars_dPhiVis_1_Mean" , &RJVars_dPhiVis_1_Mean,"RJVars_dPhiVis_1_Mean/F");                
		// output->tree()->Branch( "RJVars_cosTheta_Tot_Mean" , &RJVars_cosTheta_Tot_Mean ,"RJVars_cosTheta_Tot_Mean/F");                   
		// output->tree()->Branch( "RJVars_cosTheta_0_Mean" , &RJVars_cosTheta_0_Mean,"RJVars_cosTheta_0_Mean/F");           
		//output->tree()->Branch( "RJVars_cosTheta_1_Mean" , &RJVars_cosTheta_1_Mean,"RJVars_cosTheta_1_Mean/F");                 
		// output->tree()->Branch( "RJVars_dPhiDecay_Tot_Mean" , &RJVars_dPhiDecay_Tot_Mean,"RJVars_dPhiDecay_Tot_Mean/F");                    
		// output->tree()->Branch( "RJVars_gamma_Tot_Mean" , &RJVars_gamma_Tot_Mean,"RJVars_gamma_Tot_Mean/F");                

		// output->tree()->Branch( "RJVars_M_Tot_Var" , &RJVars_M_Tot_Var,"RJVars_M_Tot_Var/F");           
		// output->tree()->Branch( "RJVars_M_0_Var" , &RJVars_M_0_Var,"RJVars_M_0_Var/F");         
		// output->tree()->Branch( "RJVars_M_1_Var" , &RJVars_M_1_Var,"RJVars_M_1_Var/F");         
		// output->tree()->Branch( "RJVars_dPhi_Tot_Var" , &RJVars_dPhi_Tot_Var,"RJVars_dPhi_Tot_Var/F");              
		// output->tree()->Branch( "RJVars_dPhi_0_Var" , &RJVars_dPhi_0_Var,"RJVars_dPhi_0_Var/F");            
		//output->tree()->Branch( "RJVars_dPhi_1_Var" , &RJVars_dPhi_1_Var,"RJVars_dPhi_1_Var/F");            
		// output->tree()->Branch( "RJVars_dPhiVis_Tot_Var" , &RJVars_dPhiVis_Tot_Var,"RJVars_dPhiVis_Tot_Var/F");                 
		// output->tree()->Branch( "RJVars_dPhiVis_0_Var" , &RJVars_dPhiVis_0_Var,"RJVars_dPhiVis_0_Var/F");               
		//output->tree()->Branch( "RJVars_dPhiVis_1_Var" , &RJVars_dPhiVis_1_Var,"RJVars_dPhiVis_1_Var/F");               
		// output->tree()->Branch( "RJVars_cosTheta_Tot_Var" , &RJVars_cosTheta_Tot_Var,"RJVars_cosTheta_Tot_Var/F");                  
		// output->tree()->Branch( "RJVars_cosTheta_0_Var" , &RJVars_cosTheta_0_Var,"RJVars_cosTheta_0_Var/F");                
		//output->tree()->Branch( "RJVars_cosTheta_1_Var" , &RJVars_cosTheta_1_Var,"RJVars_cosTheta_1_Var/F");                
		// output->tree()->Branch( "RJVars_dPhiDecay_Tot_Var" , &RJVars_dPhiDecay_Tot_Var ,"RJVars_dPhiDecay_Tot_Var/F");                   
		// output->tree()->Branch( "RJVars_gamma_Tot_Var" , &RJVars_gamma_Tot_Var     ,"RJVars_gamma_Tot_Var/F");                         



		output->tree()->Branch( "isEE", &isEE  , "isEE/I");       
		output->tree()->Branch( "isMuMu", &isMuMu  , "isMuMu/I");       
		output->tree()->Branch( "isEMu", &isEMu  , "isEMu/I");       

		output->tree()->Branch( "isEE", &isEE  , "isEE/I");       
		output->tree()->Branch( "isMuMu", &isMuMu  , "isMuMu/I");       
		output->tree()->Branch( "isEMu", &isEMu  , "isEMu/I");       

		output->tree()->Branch("el_selected", &el_selected   );
		output->tree()->Branch("mu_selected", &mu_selected   );
		output->tree()->Branch("jet_selected", &jet_selected);
		output->tree()->Branch("bjet_selected", &bjet_selected);


		output->tree()->Branch( "TRUTH_pt",  &TRUTH_pt );
		output->tree()->Branch( "TRUTH_eta", &TRUTH_eta);
		output->tree()->Branch( "TRUTH_phi", &TRUTH_phi);
		output->tree()->Branch( "TRUTH_m",   &TRUTH_m  );
		output->tree()->Branch( "TRUTH_pdgId",   &TRUTH_pdgId  );


		output->tree()->Branch("JET_btag",   &JET_btag);
		output->tree()->Branch("JET_mass",   &JET_mass);
		output->tree()->Branch("JET_phi",   &JET_phi);
		output->tree()->Branch("JET_eta",   &JET_eta);
		output->tree()->Branch("JET_pt",   &JET_pt);

		output->tree()->Branch("LEP_charge",   &LEP_charge);
		output->tree()->Branch("LEP_E",   &LEP_E);
		output->tree()->Branch("LEP_phi",   &LEP_phi);
		output->tree()->Branch("LEP_eta",   &LEP_eta);
		output->tree()->Branch("LEP_pt",   &LEP_pt);

		// output->tree()->Branch("EL_charge",   &EL_charge);
		// output->tree()->Branch("EL_E",   &EL_E);
		// output->tree()->Branch("EL_phi",   &EL_phi);
		// output->tree()->Branch("EL_eta",   &EL_eta);
		// output->tree()->Branch("EL_pt",   &EL_pt);

		// output->tree()->Branch("MU_charge",   &MU_charge);
		// output->tree()->Branch("MU_E",   &MU_E);
		// output->tree()->Branch("MU_phi",   &MU_phi);
		// output->tree()->Branch("MU_eta",   &MU_eta);
		// output->tree()->Branch("MU_pt",   &MU_pt);

		output->tree()->Branch("MET_phi",   &MET_phi);
		output->tree()->Branch("MET",   &MET);


		output->tree()->Branch("weight", &weight);
		// output->tree()->Branch("nEvents", &nEvents);

	}



	//-- d3pd reader read branches from input tree
	event->ReadFrom( inputTree ); 

	// decide which branches to activate, and write to the output skimmed D3PD
	// event->SetActive( kFALSE, "*");

	event->SetActive( kTRUE, "RunNumber");
	event->SetActive( kTRUE, "EventNumber");
	event->SetActive( kTRUE, "lbn");
	event->SetActive( kTRUE, "isSimulation");
	event->SetActive( kTRUE, "^mcevt_weight$");

	event->SetActive( kTRUE, "^vx_n$"); 
	event->SetActive( kTRUE, "^vx_nTracks$"); 

	event->SetActive( kTRUE, "^el_n$");  // notation if you want the exact branch name
	event->SetActive( kTRUE, "^el_pt$");
	event->SetActive( kTRUE, "^el_eta$");
	event->SetActive( kTRUE, "^el_phi$");
	event->SetActive( kTRUE, "^el_E$");
	event->SetActive( kTRUE, "^el_charge$");
	event->SetActive( kTRUE, "^mu_staco_n$");  // notation if you want the exact branch name
	event->SetActive( kTRUE, "^mu_staco_pt$");
	event->SetActive( kTRUE, "^mu_staco_eta$");
	event->SetActive( kTRUE, "^mu_staco_phi$");
	event->SetActive( kTRUE, "^mu_staco_E$");
	event->SetActive( kTRUE, "^mu_staco_charge$");


	// turn on all jet branches first, then turn off some specific jet branches
	// event->SetActive( kFALSE, "jet*");
	event->SetActive( kTRUE, "^jet_AntiKt4LCTopo_pt$");
	event->SetActive( kTRUE, "^jet_AntiKt4LCTopo_eta$");
	event->SetActive( kTRUE, "^jet_AntiKt4LCTopo_phi$");
	event->SetActive( kTRUE, "^jet_AntiKt4LCTopo_E$");
	event->SetActive( kTRUE, "^jet_AntiKt4LCTopo_flavor_truth_label$");
	event->SetActive( kTRUE, "^jet_AntiKt4LCTopo_flavor_weight_MV1$");
	// event->SetActive( kTRUE, "jet_AntiKt4LCTopo_.*");
	// event->SetActive( kFALSE, "jet_AntiKt4LCTopo_MET.*");
	// event->SetActive( kFALSE, "jet_AntiKt4LCTopo_flavor.*");

	event->SetActive( kTRUE, "^jet_AntiKt4TruthJets_pt$");
	event->SetActive( kTRUE, "^jet_AntiKt4TruthJets_eta$");
	event->SetActive( kTRUE, "^jet_AntiKt4TruthJets_phi$");
	event->SetActive( kTRUE, "^jet_AntiKt4TruthJets_E$");
	event->SetActive( kTRUE, "^jet_AntiKt4TruthJets_m$");
	event->SetActive( kTRUE, "^jet_AntiKt4TruthJets_flavor_truth_label$");

	event->SetActive( kTRUE, "^MET_RefFinal_Egamma10NoTau_phi$");
	event->SetActive( kTRUE, "^MET_RefFinal_Egamma10NoTau_sumet$");

	event->SetActive( kTRUE, "^MET_Truth_Int_sumet$");
	event->SetActive( kTRUE, "^MET_Truth_Int_phi$");

	

	// Make sure that all variables that we want to copy to the output are read into memory
	event->ReadAllActive(); 

	event->WriteTo(output->tree());

	std::cout << "Leaving SklimmerAnalysis :: initialize ()"  << std::endl;


	return EL::StatusCode::SUCCESS;
}



EL::StatusCode SklimmerAnalysis :: execute ()
{
	// Here you do everything that needs to be done on every single
	// events, e.g. read input variables, apply cuts, and fill
	// histograms and trees.  This is where most of your actual analysis
	// code will go.

	int passEvent = 1;

	h_nevents->Fill(0);

	// std::cout << "started execute()"  << std::endl;



	// Important tools
	// GRL Reader
	// Pileup Reweighting Stuff
	// Event Weight

	// Get the electrons
	// containers - baseline and signal and trigmatch

	
	std::vector<int > el_baseline;
	std::vector<int > el_crack;
	std::vector<int > el_met;
	std::vector<int > el_signal;

	std::vector<int > mu_baseline;
	std::vector<int > mu_cosmic;
	std::vector<int > mu_bad;
	std::vector<int > mu_met;
	std::vector<int > mu_signal;

	std::vector<int > jet_signal;
	std::vector<int > jet_good;
	std::vector<int > jet_bad;
	std::vector<int > jet_LArHoleVeto;
	std::vector<int > jet_btagged;

	float jetptcut = 10000.;
	float elecptcut = 5000.;
	float muonptcut = 5000.;


	int iEl=0;
	for( iEl = 0; iEl < event->el.n(); iEl++  ){

		if( m_susy_obj->FillElectron(iEl,
									event->el[iEl].eta(),
									event->el[iEl].phi(),
									event->el[iEl].cl_eta(),
									event->el[iEl].cl_phi(),
									event->el[iEl].cl_E(),
									event->el[iEl].tracketa(),
									event->el[iEl].trackphi(),
									event->el[iEl].author(),
									event->el[iEl].mediumPP(),
									event->el[iEl].OQ(),
									event->el[iEl].nPixHits(),
									event->el[iEl].nSCTHits(),
									event->el_MET_Egamma10NoTau[iEl].wet().at(0),
									elecptcut,2.47,whichsyst) ){
			el_baseline.push_back(iEl);
		}

		if( event->el_MET_Egamma10NoTau[iEl].wet().at(0) != 0. ){
			el_met.push_back(iEl);
		}

	}



	for( unsigned int iEl_baseline = 0; iEl_baseline < el_baseline.size(); iEl_baseline++){
		iEl = el_baseline.at(iEl_baseline);

		if (m_susy_obj->IsInCrack( event->el[iEl].cl_eta() ))  {
			el_crack.push_back(iEl);
		}
		if(m_susy_obj->IsSignalElectronExp(iEl, 
											event->el[iEl].tightPP(), 
											event->vxp.nTracks(), 
											event->el[iEl].ptcone30(), 
											event->el[iEl].topoEtcone30_corrected(), 
											event->el[iEl].trackIPEstimate_d0_unbiasedpvunbiased(), 
											event->el[iEl].trackIPEstimate_z0_unbiasedpvunbiased(), 
											event->el[iEl].trackIPEstimate_sigd0_unbiasedpvunbiased(), 
											SignalIsoExp::TightIso) ){
			el_signal.push_back(iEl);
		}
	}




	int iMu=0;
	for( iMu = 0; iMu < event->mu_staco.n(); iMu++  ){

		if( m_susy_obj->FillMuon(iMu,
							  event->mu_staco[iMu].pt(),
							  event->mu_staco[iMu].eta(),
							  event->mu_staco[iMu].phi(),
							  event->mu_staco[iMu].me_qoverp_exPV(),
							  event->mu_staco[iMu].id_qoverp_exPV(),
							  event->mu_staco[iMu].me_theta_exPV(),
							  event->mu_staco[iMu].id_theta_exPV(),
							  event->mu_staco[iMu].id_theta(),
							  event->mu_staco[iMu].charge(),
							  event->mu_staco[iMu].isCombinedMuon(),
							  event->mu_staco[iMu].isSegmentTaggedMuon(),
							  event->mu_staco[iMu].loose(),
							  event->mu_staco[iMu].nPixHits(),
							  event->mu_staco[iMu].nPixelDeadSensors(),
							  event->mu_staco[iMu].nPixHoles(),
							  event->mu_staco[iMu].nSCTHits(),
							  event->mu_staco[iMu].nSCTDeadSensors(),
							  event->mu_staco[iMu].nSCTHoles(),
							  event->mu_staco[iMu].nTRTHits(),
							  event->mu_staco[iMu].nTRTOutliers(),
							  muonptcut,2.5, whichsyst) ){
			mu_met.push_back(iMu);
			mu_baseline.push_back(iMu);
		}
	}

	for( unsigned int iMu_baseline = 0; iMu_baseline < mu_baseline.size(); iMu_baseline++){
		iMu = mu_baseline.at(iMu_baseline);

		if (m_susy_obj->IsSignalMuonExp(iMu,
										event->vxp.nTracks(),
										event->mu_staco[iMu].ptcone30_trkelstyle(),
										event->mu_staco[iMu].etcone30(),
										event->mu_staco[iMu].trackIPEstimate_d0_unbiasedpvunbiased(),
										event->mu_staco[iMu].trackIPEstimate_z0_unbiasedpvunbiased(),
										event->mu_staco[iMu].trackIPEstimate_sigd0_unbiasedpvunbiased(),
										SignalIsoExp::TightIso)){
			mu_signal.push_back(iMu);
		}
		if(m_susy_obj->IsCosmicMuon(event->mu_staco[iMu].z0_exPV(),event->mu_staco[iMu].d0_exPV(),1.,0.2) ){
			mu_cosmic.push_back(iMu);
		}
		if( m_susy_obj->IsBadMuon(event->mu_staco[iMu].qoverp_exPV(),event->mu_staco[iMu].cov_qoverp_exPV() ) ){
			mu_bad.push_back(iMu);
		}

	}




	int iJet=0;
	int local_truth_flavor=0;
	for( iJet = 0; iJet < event->jet_AntiKt4LCTopo.n(); iJet++  ){

		m_susy_obj->FillJet(iJet,
								event->jet_AntiKt4LCTopo[iJet].pt(),
								event->jet_AntiKt4LCTopo[iJet].eta(),
								event->jet_AntiKt4LCTopo[iJet].phi(),
								event->jet_AntiKt4LCTopo[iJet].E(),
								event->jet_AntiKt4LCTopo[iJet].constscale_eta(),
								event->jet_AntiKt4LCTopo[iJet].constscale_phi(),
								event->jet_AntiKt4LCTopo[iJet].constscale_E(),
								event->jet_AntiKt4LCTopo[iJet].constscale_m(),
								event->jet_AntiKt4LCTopo[iJet].ActiveAreaPx(),
								event->jet_AntiKt4LCTopo[iJet].ActiveAreaPy(),
								event->jet_AntiKt4LCTopo[iJet].ActiveAreaPz(),
								event->jet_AntiKt4LCTopo[iJet].ActiveAreaE(),
								event->Eventshape.rhoKt4LC(),  
								event->eventinfo.averageIntPerXing(),
								event->vxp.nTracks() );

		if(whichsyst!=SystErr::NONE && !isData){
			local_truth_flavor=0;
			local_truth_flavor = event->jet_AntiKt4LCTopo[iJet].flavor_truth_label();    
			m_susy_obj->ApplyJetSystematics(iJet,event->jet_AntiKt4LCTopo[iJet].constscale_eta(),local_truth_flavor,event->eventinfo.averageIntPerXing(),event->vxp.nTracks(),whichsyst);
		}

		if(m_susy_obj->GetJetTLV(iJet).Pt() <= jetptcut) continue;
		
		bool isoverlap = false;
		for( unsigned int iEl_baseline = 0; iEl_baseline < el_baseline.size(); iEl_baseline++){
			iEl = el_baseline.at(iEl_baseline);
			if( m_susy_obj->GetElecTLV(iEl).DeltaR(m_susy_obj->GetJetTLV(iJet)) > 0.2 ) continue;
			isoverlap = true;
			break;
		}
		if(isoverlap) continue;

		bool isgoodjet = false;
		if( m_susy_obj->IsGoodJet( iJet,
									event->jet_AntiKt4LCTopo[iJet].constscale_eta(),
									event->jet_AntiKt4LCTopo[iJet].emfrac(),
									event->jet_AntiKt4LCTopo[iJet].hecf(),
									event->jet_AntiKt4LCTopo[iJet].LArQuality(),
									event->jet_AntiKt4LCTopo[iJet].HECQuality(),
									event->jet_AntiKt4LCTopo[iJet].AverageLArQF(),
									event->jet_AntiKt4LCTopo[iJet].Timing(),
									event->jet_AntiKt4LCTopo[iJet].sumPtTrk_pv0_500MeV(),
									event->jet_AntiKt4LCTopo[iJet].fracSamplingMax(),
									event->jet_AntiKt4LCTopo[iJet].SamplingMax(),
									event->jet_AntiKt4LCTopo[iJet].NegativeE(),
									event->eventinfo.RunNumber(),20000.,10.) ) isgoodjet = true;

		if(m_susy_obj->GetJetTLV(iJet).Pt() <= jetptcut) continue;

		if(!isgoodjet) jet_bad.push_back(iJet);
		else if(fabs(m_susy_obj->GetJetTLV(iJet).Eta() ) < 2.8 ){
			jet_good.push_back(iJet);
		
			if( fabs(m_susy_obj->GetJetTLV(iJet).Eta() ) < 2.5  && fabs(m_susy_obj->GetJetTLV(iJet).Pt() ) > 25000.  ){
				if( m_susy_obj->IsBJet(event->jet_AntiKt4LCTopo[iJet].flavor_weight_MV1() , 0.7892) ){
					jet_btagged.push_back(iJet);
				}
			}

		}

	}


	// Now look at all the chosen leptons and make sure they're >0.4 away from any good jet


	for( unsigned int iEl_signal = 0; iEl_signal < el_signal.size(); iEl_signal++){
		iEl = el_signal.at(iEl_signal);

		bool isoverlap = false;
		for( unsigned int iJet_good = 0; iJet_good < jet_good.size(); iJet_good++){
			iJet = jet_good[iJet_good];
			if( m_susy_obj->GetElecTLV(iEl).DeltaR(m_susy_obj->GetJetTLV(iJet)) < 0.4 ){
				isoverlap = true;
				break;
			}
		}
		if(isoverlap){
			el_signal.erase(el_signal.begin() + iEl_signal);
			iEl_signal--;
		}
	}


	for( unsigned int iMu_signal = 0; iMu_signal < mu_signal.size(); iMu_signal++){
		iMu = mu_signal.at(iMu_signal);

		bool isoverlap = false;
		for( unsigned int iJet_good = 0; iJet_good < jet_good.size(); iJet_good++){
			iJet = jet_good[iJet_good];
			if( m_susy_obj->GetMuonTLV(iMu).DeltaR(m_susy_obj->GetJetTLV(iJet)) < 0.4 ){
				isoverlap = true;
				break;
			}
		}
		if(isoverlap){
			mu_signal.erase(mu_signal.begin() + iMu_signal);
			iMu_signal--;
		}
	}

	if(event->mcevt.weight()[0].size() ){
		weight = event->mcevt.weight()[0][0][0];
	} else {
		weight = 1.;
	}

	isEE = 0;
	isMuMu = 0;
	isEMu = 0;


	TLorentzVector Lepton[3];

	TLorentzVector Jet[3];

	if( jet_btagged.size() < 1 ) return EL::StatusCode::SUCCESS;;
	if( jet_good.size() < 2 ) return EL::StatusCode::SUCCESS;;
	Jet[0] = m_susy_obj->GetJetTLV( jet_good[0] );
	Jet[1] = m_susy_obj->GetJetTLV( jet_good[1] );

	int LeptonCharge0 = 0;
	int LeptonCharge1 = 0;

	// if( el_signal.size()==2 ){
	// 	isEE = 1;
	// 	Lepton[0] = m_susy_obj->GetElecTLV( el_signal[0] );
	// 	Lepton[1] = m_susy_obj->GetElecTLV( el_signal[1] );
	// 	LeptonCharge0 = event->el[el_signal[0] ].charge();
	// 	LeptonCharge1 = event->el[el_signal[1] ].charge();
	// } else if ( mu_signal.size()==2 ){
	// 	isMuMu = 1;
	// 	Lepton[0] = m_susy_obj->GetMuonTLV( mu_signal[0] );
	// 	Lepton[1] = m_susy_obj->GetMuonTLV( mu_signal[1] );
	// 	LeptonCharge0 = event->mu_staco[mu_signal[0] ].charge();
	// 	LeptonCharge1 = event->mu_staco[mu_signal[1] ].charge();
	// } else if ( mu_signal.size()==1 && el_signal.size()==1 ){
	// 	isEMu = 1;
	// 	Lepton[0] = m_susy_obj->GetElecTLV( el_signal[0] );
	// 	Lepton[1] = m_susy_obj->GetMuonTLV( mu_signal[0] );
	// 	LeptonCharge0 = event->el[el_signal[0] ].charge();
	// 	LeptonCharge1 = event->mu_staco[mu_signal[0] ].charge();
	// } else return EL::StatusCode::SUCCESS;

	// if( (Lepton[0]+Jet[0]).M() + (Lepton[1]+Jet[1]).M()  >  (Lepton[1]+Jet[0]).M() + (Lepton[0]+Jet[1]).M()  ){
	// 	Lepton[3] = Lepton[0];
	// 	Lepton[0] = Lepton[1];
	// 	Lepton[1] = Lepton[3];
	// }

	TVector2 met = m_susy_obj->GetMET(
									event->jet_AntiKt4LCTopo_MET_Egamma10NoTau.wet(),
									event->jet_AntiKt4LCTopo_MET_Egamma10NoTau.wpx(),
									event->jet_AntiKt4LCTopo_MET_Egamma10NoTau.wpy(),
									event->jet_AntiKt4LCTopo_MET_Egamma10NoTau.statusWord(),
									el_met,
									event->el_MET_Egamma10NoTau.wet(),
									event->el_MET_Egamma10NoTau.wpx(),
									event->el_MET_Egamma10NoTau.wpy(),
									event->el_MET_Egamma10NoTau.statusWord(),                             
									event->MET_CellOut_Egamma10NoTau.etx(), 
									event->MET_CellOut_Egamma10NoTau.ety(),
									event->MET_CellOut_Egamma10NoTau.sumet(),
									event->MET_CellOut_Eflow_STVF_Egamma10NoTau.etx(), 
									event->MET_CellOut_Eflow_STVF_Egamma10NoTau.ety(),
									event->MET_CellOut_Eflow_STVF_Egamma10NoTau.sumet(),                                
									event->MET_RefGamma_Egamma10NoTau.etx(),
									event->MET_RefGamma_Egamma10NoTau.ety(),
									event->MET_RefGamma_Egamma10NoTau.sumet(),
									mu_met,
									event->mu_staco.ms_qoverp(), 
									event->mu_staco.ms_theta(), 
									event->mu_staco.ms_phi(), 
									event->mu_staco.charge(),
									event->mu_staco.energyLossPar(),
									event->eventinfo.averageIntPerXing(),
									SUSYMet::Default,
									whichsyst,
									false);


	RJTool->newEvent();

	RJTool->addVisParticle("b",Jet[0],1);
	RJTool->addVisParticle("b",Jet[1],2);

	// RJTool->addVisParticle("l",Lepton[0],1);
	// RJTool->addVisParticle("l",Lepton[1],2);

	if( met.X() == 0. && met.Y() == 0. ) return EL::StatusCode::SUCCESS;
	TVector3 METVector;
	METVector.SetXYZ(  met.X(), met.Y(), 0.0 );
	RJTool->addMET( METVector );

	RJTool->setHemisphereMode(0); //top symmetry
	RJTool->guessInvParticles();
	RJTool->getObservables();

	// If you want access to the variables to do whatever with...

	// Try again with other Hemisphere mode

	// RJTool->setHemisphereMode(1); //W symmetry
	// RJTool->guessInvParticles();
	// RJTool->getObservables();

	std::map< TString, double > RJVars = RJTool->getObservablesMap();

	// RJVars_sqrtsHat_0_0_0 = RJVars["sqrtsHat_0_0_0"];
	// RJVars_invgamma_0_0_0 = RJVars["invgamma_0_0_0"];
	// RJVars_mdelta_0_0_0 = RJVars["mdelta_0_0_0"];
	// RJVars_cospt_0_0_0 = RJVars["cospt_0_0_0"];

	RJVars_sHatR_0_0_0 = RJVars["sHatR_0_0_0"];  
	RJVars_E12_0_0_0 = RJVars["E12_0_0_0"];  
	RJVars_gamma_R_0_0_0 = RJVars["gamma_R_0_0_0"];  
	RJVars_dphi_Beta_R_0_0_0 = RJVars["dphi_Beta_R_0_0_0"];  
	RJVars_dphi_leg1_leg2_0_0_0 = RJVars["dphi_leg1_leg2_0_0_0"];  
	RJVars_gamma_Rp1_0_0_0 = RJVars["gamma_Rp1_0_0_0"];  
	RJVars_dphi_Beta_Rp1_Beta_R_0_0_0 = RJVars["dphi_Beta_Rp1_Beta_R_0_0_0"];  
	RJVars_MDeltaR_0_0_0 = RJVars["MDeltaR_0_0_0"];  
	RJVars_Eleg1_0_0_0 = RJVars["Eleg1_0_0_0"];  
	RJVars_Eleg2_0_0_0 = RJVars["Eleg2_0_0_0"];  
	RJVars_costhetaRp1_0_0_0 = RJVars["costhetaRp1_0_0_0"];  
	RJVars_costhetaR_0_0_0 = RJVars["costhetaR_0_0_0"];  

	// Filling branches for slims for crogan/jack

	//MET
	MET = METVector.Mag();
	MET_phi = METVector.Phi();

	// Leptons
	std::vector<float> tmpLEP_pt;
	std::vector<float> tmpLEP_eta;
	std::vector<float> tmpLEP_phi;
	std::vector<float> tmpLEP_E;
	std::vector<float> tmpLEP_charge;

	tmpLEP_pt.push_back(Lepton[0].Pt() );
	tmpLEP_eta.push_back(Lepton[0].Eta() );
	tmpLEP_phi.push_back(Lepton[0].Phi() );
	tmpLEP_E.push_back(Lepton[0].E() );
	tmpLEP_charge.push_back(LeptonCharge0 );

	tmpLEP_pt.push_back(Lepton[1].Pt() );
	tmpLEP_eta.push_back(Lepton[1].Eta() );
	tmpLEP_phi.push_back(Lepton[1].Phi() );
	tmpLEP_E.push_back(Lepton[1].E() );
	tmpLEP_charge.push_back(LeptonCharge1 );

	LEP_pt = new std::vector<float>(tmpLEP_pt);
	LEP_eta= new std::vector<float>(tmpLEP_eta);
	LEP_phi= new std::vector<float>(tmpLEP_phi);
	LEP_E  = new std::vector<float>(tmpLEP_E);
	LEP_charge = new std::vector<float>(tmpLEP_charge);

	// Jets
	std::vector<float> tmpJET_btag;
	std::vector<float> tmpJET_mass;
	std::vector<float> tmpJET_phi;
	std::vector<float> tmpJET_eta;
	std::vector<float> tmpJET_pt ;

	TLorentzVector tmpTLV;
	for( unsigned int iJet_good = 0; iJet_good < jet_good.size(); iJet_good++){
		iJet = jet_good[iJet_good];
		tmpTLV = m_susy_obj->GetJetTLV(iJet);

		tmpJET_pt.push_back(tmpTLV.Pt() );
		tmpJET_eta.push_back(tmpTLV.Eta() );
		tmpJET_phi.push_back(tmpTLV.Phi() );
		tmpJET_mass.push_back(tmpTLV.M() );
		if( std::find(jet_btagged.begin(), jet_btagged.end(), iJet)!=jet_btagged.end() ){
			tmpJET_btag.push_back(   1.  );
		} else {
			tmpJET_btag.push_back(   0.  );
		}
	}

	JET_pt = new std::vector<float>(tmpJET_pt);
	JET_eta = new std::vector<float>(tmpJET_eta);
	JET_phi = new std::vector<float>(tmpJET_phi);
	JET_mass = new std::vector<float>(tmpJET_mass);
	JET_btag = new std::vector<float>(tmpJET_btag);

	//now need to do the truth stuff which is reasonably harder...

	//MET_TRUTH
	MET_TRUTH = event->MET_Truth.Int_et();
	MET_TRUTH_phi = event->MET_Truth.Int_phi();


	std::vector<int> truth_index;

	std::vector<int> pdgIdsToSave;

	// pdgIdsToSave.push_back(5);
	pdgIdsToSave.push_back(6);
	// pdgIdsToSave.push_back(11);
	// pdgIdsToSave.push_back(12);
	// pdgIdsToSave.push_back(13);
	// pdgIdsToSave.push_back(14);
	// pdgIdsToSave.push_back(15);
	// pdgIdsToSave.push_back(16);
	pdgIdsToSave.push_back(24);
	pdgIdsToSave.push_back(1000006);
	pdgIdsToSave.push_back(2000006);
	pdgIdsToSave.push_back(1000022);
	pdgIdsToSave.push_back(1000024);


	int iMC=0;
	for( iMC = 0; iMC < event->mc.n(); iMC++){

		//first stuff that has more complicated parentage requirements
		if( event->mc[iMC].status()==1 && (fabs(event->mc[iMC].pdgId() )==11 || fabs(event->mc[iMC].pdgId() )==13  ) ){
			bool saveThisLepton = hasAncestor(iMC,24);
			if(saveThisLepton){
				truth_index.push_back(iMC);
			}
		}
		if( event->mc[iMC].status()==1 && \
			(fabs(event->mc[iMC].pdgId() )== 12 || \
			fabs(event->mc[iMC].pdgId() )== 14 || \
			fabs(event->mc[iMC].pdgId() )== 16 ) ) truth_index.push_back(iMC);

		if( fabs(event->mc[iMC].pdgId() )== 5 ){
			bool saveThisB = hasAncestor(iMC,6);
			if(saveThisB){
				truth_index.push_back(iMC);
			}
		}

		if( std::find(pdgIdsToSave.begin(), pdgIdsToSave.end(), fabs(event->mc[iMC].pdgId() ) ) !=pdgIdsToSave.end() ){
			truth_index.push_back(iMC);
		}

	}

	std::vector<float> tmpTRUTH_pdgId;
	std::vector<float> tmpTRUTH_charge;
	std::vector<float> tmpTRUTH_pt;
	std::vector<float> tmpTRUTH_eta;
	std::vector<float> tmpTRUTH_phi;
	std::vector<float> tmpTRUTH_m;

	for( unsigned int iMC_index = 0; iMC_index < truth_index.size(); iMC_index++){
		iMC = truth_index[iMC_index];
		tmpTRUTH_pdgId.push_back( event->mc[iMC].pdgId() );
		tmpTRUTH_charge.push_back( event->mc[iMC].charge() );
		tmpTRUTH_pt.push_back( event->mc[iMC].pt() );
		tmpTRUTH_eta.push_back( event->mc[iMC].eta() );
		tmpTRUTH_phi.push_back( event->mc[iMC].phi() );
		tmpTRUTH_m.push_back( event->mc[iMC].m() );
	}

	TRUTH_pdgId   = new std::vector<float> (tmpTRUTH_pdgId   );          
	TRUTH_charge   = new std::vector<float> (tmpTRUTH_charge   );          
	TRUTH_pt   = new std::vector<float> (tmpTRUTH_pt   );          
	TRUTH_eta  = new std::vector<float> (tmpTRUTH_eta  );         
	TRUTH_phi  = new std::vector<float> (tmpTRUTH_phi  );         
	TRUTH_m    = new std::vector<float> (tmpTRUTH_m    );       


	// RJVars_M_0_0_0              =  RJVars["M_0_0_0"];
	// RJVars_M_0_0_1              =  RJVars["M_0_0_1"];
	// RJVars_M_1_0_0              =  RJVars["M_1_0_0"];
	// RJVars_M_1_0_1              =  RJVars["M_1_0_1"];
	// RJVars_M_1_1_0              =  RJVars["M_1_1_0"];
	// RJVars_M_1_1_1              =  RJVars["M_1_1_1"];
	// RJVars_M_2_0_0              =  RJVars["M_2_0_0"];
	// RJVars_M_2_0_1              =  RJVars["M_2_0_1"];
	// RJVars_M_2_1_0              =  RJVars["M_2_1_0"];
	// RJVars_M_2_1_1              =  RJVars["M_2_1_1"];
	// RJVars_MDecay_1_1_0         =  RJVars["MDecay_1_1_0"];
	// RJVars_MDecay_1_1_1         =  RJVars["MDecay_1_1_1"];
	// RJVars_MDecay_2_1_0         =  RJVars["MDecay_2_1_0"];
	// RJVars_MDecay_2_1_1         =  RJVars["MDecay_2_1_1"];
	// RJVars_dPhi_0_0_0         =  RJVars["dPhi_0_0_0"];
	// RJVars_dPhi_0_0_1         =  RJVars["dPhi_0_0_1"];
	// RJVars_dPhi_1_0_0         =  RJVars["dPhi_1_0_0"];
	// RJVars_dPhi_1_0_1         =  RJVars["dPhi_1_0_1"];
	// RJVars_dPhi_2_0_0         =  RJVars["dPhi_2_0_0"];
	// RJVars_dPhi_2_0_1         =  RJVars["dPhi_2_0_1"];
	// RJVars_dPhi_1_1_0         =  RJVars["dPhi_1_1_0"];
	// RJVars_dPhi_1_1_1         =  RJVars["dPhi_1_1_1"];
	// RJVars_dPhi_2_1_0         =  RJVars["dPhi_2_1_0"];
	// RJVars_dPhi_2_1_1         =  RJVars["dPhi_2_1_1"];
	// RJVars_dPhiVis_0_0_0         =  RJVars["dPhiVis_0_0_0"];
	// RJVars_dPhiVis_0_0_1         =  RJVars["dPhiVis_0_0_1"];
	// RJVars_dPhiVis_1_0_0         =  RJVars["dPhiVis_1_0_0"];
	// RJVars_dPhiVis_1_0_1         =  RJVars["dPhiVis_1_0_1"];
	// RJVars_dPhiVis_2_0_0         =  RJVars["dPhiVis_2_0_0"];
	// RJVars_dPhiVis_2_0_1         =  RJVars["dPhiVis_2_0_1"];
	// RJVars_dPhiVis_1_1_0         =  RJVars["dPhiVis_1_1_0"];
	// RJVars_dPhiVis_1_1_1         =  RJVars["dPhiVis_1_1_1"];
	// RJVars_dPhiVis_2_1_0         =  RJVars["dPhiVis_2_1_0"];
	// RJVars_dPhiVis_2_1_1         =  RJVars["dPhiVis_2_1_1"];
	// RJVars_cosTheta_0_0_0         =  RJVars["cosTheta_0_0_0"];
	// RJVars_cosTheta_0_0_1         =  RJVars["cosTheta_0_0_1"];
	// RJVars_cosTheta_1_0_0         =  RJVars["cosTheta_1_0_0"];
	// RJVars_cosTheta_1_0_1         =  RJVars["cosTheta_1_0_1"];
	// RJVars_cosTheta_2_0_0         =  RJVars["cosTheta_2_0_0"];
	// RJVars_cosTheta_2_0_1         =  RJVars["cosTheta_2_0_1"];
	// RJVars_cosTheta_1_1_0         =  RJVars["cosTheta_1_1_0"];
	// RJVars_cosTheta_1_1_1         =  RJVars["cosTheta_1_1_1"];
	// RJVars_cosTheta_2_1_0         =  RJVars["cosTheta_2_1_0"];
	// RJVars_cosTheta_2_1_1         =  RJVars["cosTheta_2_1_1"];
	// RJVars_dPhiDecay_0_0_0         =  RJVars["dPhiDecay_0_0_0"];
	// RJVars_dPhiDecay_0_0_1         =  RJVars["dPhiDecay_0_0_1"];
	// RJVars_dPhiDecay_1_0_0         =  RJVars["dPhiDecay_1_0_0"];
	// RJVars_dPhiDecay_1_0_1         =  RJVars["dPhiDecay_1_0_1"];
	// RJVars_dPhiDecay_2_0_0         =  RJVars["dPhiDecay_2_0_0"];
	// RJVars_dPhiDecay_2_0_1         =  RJVars["dPhiDecay_2_0_1"];
	// RJVars_cosThetaDecay_0_0_0         =  RJVars["cosThetaDecay_0_0_0"];
	// RJVars_cosThetaDecay_0_0_1         =  RJVars["cosThetaDecay_0_0_1"];
	// RJVars_cosThetaDecay_1_0_0         =  RJVars["cosThetaDecay_1_0_0"];
	// RJVars_cosThetaDecay_1_0_1         =  RJVars["cosThetaDecay_1_0_1"];
	// RJVars_cosThetaDecay_2_0_0         =  RJVars["cosThetaDecay_2_0_0"];
	// RJVars_cosThetaDecay_2_0_1         =  RJVars["cosThetaDecay_2_0_1"];
	// RJVars_gamma_0_0_0         =  RJVars["gamma_0_0_0"];
	// RJVars_gamma_0_0_1         =  RJVars["gamma_0_0_1"];


 	// RJVars_M_Tot_Mean = getMean(RJVars_M_0_0_0, RJVars_M_0_0_1);
 	// RJVars_M_0_Mean = getMean(RJVars_M_1_0_0, RJVars_M_2_0_0, RJVars_M_1_0_1, RJVars_M_2_0_1);
 	// RJVars_M_1_Mean = getMean(RJVars_M_1_1_0, RJVars_M_2_1_0, RJVars_M_1_1_1, RJVars_M_2_1_1);
 	// RJVars_dPhi_Tot_Mean = getMean(RJVars_dPhi_0_0_0, RJVars_dPhi_0_0_1);
 	// RJVars_dPhi_0_Mean = getMean(RJVars_dPhi_1_0_0, RJVars_dPhi_2_0_0, RJVars_dPhi_1_0_1, RJVars_dPhi_2_0_1);
 	// RJVars_dPhi_1_Mean = getMean(RJVars_dPhi_1_1_0, RJVars_dPhi_2_1_0, RJVars_dPhi_1_1_1, RJVars_dPhi_2_1_1);
 	// RJVars_dPhiVis_Tot_Mean = getMean(RJVars_dPhiVis_0_0_0, RJVars_dPhiVis_0_0_1);
 	// RJVars_dPhiVis_0_Mean = getMean(RJVars_dPhiVis_1_0_0, RJVars_dPhiVis_2_0_0, RJVars_dPhiVis_1_0_1, RJVars_dPhiVis_2_0_1);
 	// RJVars_dPhiVis_1_Mean = getMean(RJVars_dPhiVis_1_1_0, RJVars_dPhiVis_2_1_0, RJVars_dPhiVis_1_1_1, RJVars_dPhiVis_2_1_1);
 	// RJVars_cosTheta_Tot_Mean = getMean(RJVars_cosTheta_0_0_0, RJVars_cosTheta_0_0_1);
 	// RJVars_cosTheta_0_Mean = getMean(RJVars_cosTheta_1_0_0, RJVars_cosTheta_2_0_0, RJVars_cosTheta_1_0_1, RJVars_cosTheta_2_0_1);
 	// RJVars_cosTheta_1_Mean = getMean(RJVars_cosTheta_1_1_0, RJVars_cosTheta_2_1_0, RJVars_cosTheta_1_1_1, RJVars_cosTheta_2_1_1);
 	// RJVars_dPhiDecay_Tot_Mean = getMean(RJVars_dPhiDecay_0_0_0, RJVars_dPhiDecay_0_0_1);
 	// RJVars_gamma_Tot_Mean = getMean(RJVars_gamma_0_0_0, RJVars_gamma_0_0_1);

 	// RJVars_M_Tot_Var = getVar(RJVars_M_0_0_0, RJVars_M_0_0_1);
 	// RJVars_M_0_Var = getVar(RJVars_M_1_0_0, RJVars_M_2_0_0, RJVars_M_1_0_1, RJVars_M_2_0_1);
 	// RJVars_M_1_Var = getVar(RJVars_M_1_1_0, RJVars_M_2_1_0, RJVars_M_1_1_1, RJVars_M_2_1_1);
 	// RJVars_dPhi_Tot_Var = getVar(RJVars_dPhi_0_0_0, RJVars_dPhi_0_0_1);
 	// RJVars_dPhi_0_Var = getVar(RJVars_dPhi_1_0_0, RJVars_dPhi_2_0_0, RJVars_dPhi_1_0_1, RJVars_dPhi_2_0_1);
 	// RJVars_dPhi_1_Var = getVar(RJVars_dPhi_1_1_0, RJVars_dPhi_2_1_0, RJVars_dPhi_1_1_1, RJVars_dPhi_2_1_1);
 	// RJVars_dPhiVis_Tot_Var = getVar(RJVars_dPhiVis_0_0_0, RJVars_dPhiVis_0_0_1);
 	// RJVars_dPhiVis_0_Var = getVar(RJVars_dPhiVis_1_0_0, RJVars_dPhiVis_2_0_0, RJVars_dPhiVis_1_0_1, RJVars_dPhiVis_2_0_1);
 	// RJVars_dPhiVis_1_Var = getVar(RJVars_dPhiVis_1_1_0, RJVars_dPhiVis_2_1_0, RJVars_dPhiVis_1_1_1, RJVars_dPhiVis_2_1_1);
 	// RJVars_cosTheta_Tot_Var = getVar(RJVars_cosTheta_0_0_0, RJVars_cosTheta_0_0_1);
 	// RJVars_cosTheta_0_Var = getVar(RJVars_cosTheta_1_0_0, RJVars_cosTheta_2_0_0, RJVars_cosTheta_1_0_1, RJVars_cosTheta_2_0_1);
 	// RJVars_cosTheta_1_Var = getVar(RJVars_cosTheta_1_1_0, RJVars_cosTheta_2_1_0, RJVars_cosTheta_1_1_1, RJVars_cosTheta_2_1_1);
 	// RJVars_dPhiDecay_Tot_Var = getVar(RJVars_dPhiDecay_0_0_0, RJVars_dPhiDecay_0_0_1);
 	// RJVars_gamma_Tot_Var = getVar(RJVars_gamma_0_0_0, RJVars_gamma_0_0_1);


	el_selected  = new vector<int>(el_signal);
	mu_selected  = new vector<int>(mu_signal);
	jet_selected = new vector<int>(jet_good );
	bjet_selected = new vector<int>(jet_btagged );


// 	// if( (event->triggerbits.EF_mu24_tight() != 1)    && 
// 	//     (event->triggerbits.EF_e24vh_medium1() != 1) && 
// 	//     (event->triggerbits.EF_xe80_tclcw() != 1) ){
// 	//   passEvent = 0;
// 	// }

	if(passEvent){
		// You can put some criteria here for the event to be selected.
		output->setFilterPassed (); // You must have this line somewhere
	}

	
	output->setFilterPassed ();
	
	// std::cout << "leaving execute()"  << std::endl;


	return EL::StatusCode::SUCCESS;
}

Int_t SklimmerAnalysis :: getOriginBarcode( int iMC, int originPDGID ){
	if( (event->mc[iMC].parent_index()).size()==0 ){ return 0; }

	int pastAncestor = 0;

	int iParentIndex=0;
	for( unsigned int iParent=0; iParent < event->mc[iMC].parent_index().size(); iParent++){
		iParentIndex = event->mc[iMC].parent_index()[iParent];
		if( fabs(event->mc[iParentIndex].pdgId() ) == originPDGID ) return event->mc[iParentIndex].barcode();

		pastAncestor = getOriginBarcode( iParentIndex, originPDGID );
		if(pastAncestor) return pastAncestor;
	}

	return 0;
}

// Int_t SklimmerAnalysis :: hasOriginBarcode( int iMC, int barcode ){
// 	if( (event->mc[iMC].parent_index()).size()==0 ){ return 0; }

// 	int pastAncestor = 0;

// 	int iParentIndex=0;
// 	for( unsigned int iParent=0; iParent < event->mc[iMC].parent_index().size(); iParent++){
// 		iParentIndex = event->mc[iMC].parent_index()[iParent];
// 		if( fabs(event->mc[iParentIndex].pdgId() ) == originPDGID ) return event->mc[iParentIndex].barcode();

// 		pastAncestor = getOriginBarcode( iParentIndex, originPDGID );
// 		if(pastAncestor) return pastAncestor;
// 	}

// 	return 0;
// }

Bool_t SklimmerAnalysis :: hasAncestor( int iMC, int ancestor ){
	if( (event->mc[iMC].parent_index()).size()==0 ){ return 0; }

	bool pastAncestor = 0;

	int iParentIndex=0;
	for( unsigned int iParent=0; iParent < event->mc[iMC].parent_index().size(); iParent++){
		iParentIndex = event->mc[iMC].parent_index()[iParent];
		if( fabs(event->mc[iParentIndex].pdgId() ) == ancestor ) return 1;

		pastAncestor = hasAncestor( iParentIndex, ancestor );
		if(pastAncestor) return 1;
	}

	return 0;
}


Float_t SklimmerAnalysis :: getMean (Float_t a, Float_t b, Float_t c, Float_t d)
{
	Float_t n=4.;
	Float_t sum=a+b+c+d;

	return sum/n;
}

Float_t SklimmerAnalysis :: getMean (Float_t a, Float_t b)
{
	Float_t n=2.;
	Float_t sum=a+b;

	return sum/n;
}

Float_t SklimmerAnalysis :: getVar (Float_t a, Float_t b, Float_t c, Float_t d)
{
  //Float_t n=4.;
	Float_t mean=getMean(a,b,c,d);
	Float_t sum = (a-mean)*(a-mean) + (b-mean)*(b-mean) + (c-mean)*(c-mean) + (d-mean)*(d-mean)   ;

	return sum / (3.*mean);
}

Float_t SklimmerAnalysis :: getVar (Float_t a, Float_t b)
{
  //Float_t n=2.;
	Float_t mean=getMean(a,b);
	Float_t sum=(a-mean)*(a-mean) + (b-mean)*(b-mean) ;

	return sum/mean;
}

void SklimmerAnalysis :: SetSyst(SystErr::Syste syst)
{
	std::cout << Form("Setting Syst To: %d",syst) << std::endl; 
	this->whichsyst = syst;
	std::cout << Form("Successfully set to: %d",this->whichsyst) << std::endl; 
	return;
}
Int_t SklimmerAnalysis :: GetSyst()
{
	std::cout << Form("GetSyst: %d",whichsyst) << std::endl; 
	return this->whichsyst;
}


EL::StatusCode SklimmerAnalysis :: postExecute ()
{
	// Here you do everything that needs to be done after the main event
	// processing.  This is typically very rare, particularly in user
	// code.  It is mainly used in implementing the NTupleSvc.
	return EL::StatusCode::SUCCESS;
}



EL::StatusCode SklimmerAnalysis :: finalize ()
{
	// This method is the mirror image of initialize(), meaning it gets
	// called after the last event has been processed on the worker node
	// and allows you to finish up any objects you created in
	// initialize() before they are written to disk.  This is actually
	// fairly rare, since this happens separately for each worker node.
	// Most of the time you want to do your post-processing on the
	// submission node after all your histogram outputs have been
	// merged.  This is different from histFinalize() in that it only
	// gets called on worker nodes that processed input events.
	return EL::StatusCode::SUCCESS;
}



EL::StatusCode SklimmerAnalysis :: histFinalize ()
{
	// This method is the mirror image of histInitialize(), meaning it
	// gets called after the last event has been processed on the worker
	// node and allows you to finish up any objects you created in
	// histInitialize() before they are written to disk.  This is
	// actually fairly rare, since this happens separately for each
	// worker node.  Most of the time you want to do your
	// post-processing on the submission node after all your histogram
	// outputs have been merged.  This is different from finalize() in
	// that it gets called on all worker nodes regardless of whether
	// they processed input events.
	return EL::StatusCode::SUCCESS;
}
