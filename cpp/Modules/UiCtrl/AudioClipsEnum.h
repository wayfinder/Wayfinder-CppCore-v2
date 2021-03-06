/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef AUDIO_CLIPS_ENUM_H
#define AUDIO_CLIPS_ENUM_H

#undef USE_OLD_ENGLISH

namespace isab{

   // Note that if the order of these change, AudioClipTable must also
   // be changed so that it is sorted.
   namespace AudioClipsEnum {

      const static int sv_base =  100;
      const static int en_base =  200;
      const static int de_base =  300;
      const static int da_base =  400;
      const static int it_base =  500;
      const static int fr_base =  600;
      const static int es_base =  700;
      const static int nl_base =  800;
      const static int fi_base =  900;
      const static int no_base = 1000;
      const static int us_base = 1100;
      const static int pt_base = 1200;
      const static int hu_base = 1300;
      const static int pl_base = 1400;
      const static int cs_base = 1500;
      const static int sl_base = 1600;

      enum Clips {
         /* Meta sounds */
         SoundEnd          = -1,
         SoundMayBreak     = -2,
         SoundTimingMarker = -3,
         SoundNoKnownSound = -4,

         /* Universal sounds (used by several languages) */
         SoundNewCrossing  = 0,

         /* Swedish */
         SvSoundSvangVanster = sv_base,
         SvSoundSvangHoger,
         SvSoundHaer,
         SvSoundOmTjugofemMeter,
         SvSoundOmFemtioMeter,
         SvSoundOmEttHundraMeter,
         SvSoundOmTvaHundraMeter,
         SvSoundOmFemHundraMeter,
         SvSoundOmEnKilometer,
         SvSoundOmTvaKilometer,
         SvSoundHaallVanster,
         SvSoundHaallHoger,
         SvSoundIRondellen,
         SvSoundDuHarAvvikitFranRutten,
         SvSoundDuKorAtFelHall,
         SvSoundDuArFrammeVidDinDest,
         SvSoundVidDestinationen,
         SvSoundKorRaktFram,
         SvSoundKorAv,
         SvSoundKorUr,
         SvSoundUtfarten,
         SvSoundTa,
         SvSoundGorEnUSvang,
         SvSoundForsta,
         SvSoundAndra,
         SvSoundTredje,
         SvSoundFjarde,
         SvSoundFemte,
         SvSoundSjatte,
         SvSoundSjunde,
         SvSoundAttonde,
         SvSoundOchDarefter,
         SvSoundDirekt,
         SvSoundCamera,


#ifdef USE_OLD_ENGLISH
         /* English */
         EnSoundTheFirst = en_base,
         EnSoundTheSecond,
         EnSoundTheThird,
         EnSoundTheFourth,
         EnSoundTheFifth,
         EnSoundTheSixth,
         EnSoundTheSeventh,
         EnSoundTheEighth,
         EnSoundAndThen,
         EnSoundAtTheDestination,
         EnSoundDriveStraightAhead,
         EnSoundExit,
         EnSoundYouHaveReachedYourDestination,
         EnSoundHere,
         EnSoundInOneHundredMeters,
         EnSoundInOneKilometer,
         EnSoundInTwohundredMeters,
         EnSoundInTwentyfiveMeters,
         EnSoundInTwoKilometers,
         EnSoundInFiftyMeters,
         EnSoundInFivehundredMeters,
         EnSoundInTheRoundabout,
         EnSoundKeepLeft,
         EnSoundKeepRight,
         EnSoundMeters,
         EnSoundTake,
         EnSoundTurnLeft,
         EnSoundTurnRight,
         EnSoundUturn,
         EnSoundYouAreDrivingInTheWrongDirection,
         EnSoundYouAreNowOffTrack,
         EnSoundCamera,
#else
         EnSoundIn100Feet = en_base,
         EnSoundIn200Feet,
         EnSoundIn500Feet,
         EnSoundInAQuarterOfAMile,
         EnSoundInHalfAMile,
         EnSoundIn1Mile,
         EnSoundIn2Miles,
         EnSoundIn2Kilometers,
         EnSoundIn1Kilometer,
         EnSoundIn500Meters,
         EnSoundIn200Meters,
         EnSoundIn100Meters,
         EnSoundIn50Meters,
         EnSoundIn25Meters,
         EnSoundIn500Yards,
         EnSoundIn200Yards,
         EnSoundIn100Yards,
         EnSoundIn50Yards,
         EnSoundIn25Yards,
         EnSoundTheFirst,
         EnSoundTheSecond,
         EnSoundTheThird,
         EnSoundTheFourth,
         EnSoundTheFifth,
         EnSoundTheSixth,
         EnSoundTheSeventh,
         EnSoundTheEighth,
         EnSoundAndThen,
         EnSoundAtTheDestination,
         EnSoundAtTheRoundabout,
         EnSoundDriveStraightAhead,
         EnSoundExitNoun,
         EnSoundExitVerb,
         EnSoundHere,
         EnSoundImmediately,
         EnSoundKeepLeft,
         EnSoundKeepRight,
         EnSoundMakeAUTurn,
         EnSoundPleaseTurnAround,
         EnSoundTake,
         EnSoundTurnLeft,
         EnSoundTurnRight,
         EnSoundYouAreNowOffTrack,
         EnSoundYouHaveReachedYourDestination,
         EnSoundCamera,
#endif

         /* German */
         DeSoundDieErste = de_base,
         DeSoundDieZweite,
         DeSoundDieDritte,
         DeSoundDieVierte,
         DeSoundDieFunfte,
         DeSoundDieSechte,
         DeSoundDieSibte,
         DeSoundDieAchrte,
         DeSoundUndDann,
         //         DeSoundDasZielBefindetSichGeradeaus,
         DeSoundAmZiel,
         DeSoundGeradeausWeiterfahren,
         DeSoundAusfahrt,
         DeSoundAbfahren,
         DeSoundSieHabenDasZielErreicht,
         DeSoundHier,
         DeSoundInFunfUndZwanzigMetern,
         DeSoundInFunfzigMetern,
         DeSoundInEinhundertMetern,
         DeSoundInZweihundertMetern,
         DeSoundInFunfhundertMetern,
         DeSoundInEinemKilometer,
         DeSoundInZweiKilometern,
         DeSoundImKreisel,
         DeSoundLinksHalten,
         DeSoundRechtsHalten,
         DeSoundLinksAbbiegen,
         DeSoundRechtsAbbiegen,
         DeSoundSieFahrenJetztInDieFalsheRichtung,
         DeSoundSieHabenJetztDieGeplanteRouteVerlassen,
         DeSoundBitteWenden,
         DeSoundWennMoglichBitteWenden,
         DeSoundSofort,
         DeSoundCamera,


         /* Danish */
         DaSoundDenForste = da_base,
         DaSoundDenAnnen,
         DaSoundDenTredje,
         DaSoundDenFjerre,
         DaSoundDenFemte,
         DaSoundDenSjaette,
         DaSoundDenSuvene,
         DaSoundDenAttande,
         DaSoundDarefter,
         DaSoundKorLigeUd,
         DaSoundVej,
         DaSoundFrakorsel,
         DaSoundDuErFrammeVidMalet,
         DaSoundNu,
         DaSoundOmEttHundredeMeter,
         DaSoundOmEnKilometer,
         DaSoundOmTuHundredeMeter,
         DaSoundOmFemOgTuveMeter,
         DaSoundOmTuKilometer,
         DaSoundOmHalvtredsMeter,
         DaSoundOmFemhundredeMeter,
         DaSoundIRundkorseln,
         DaSoundHallTillVenstre,
         DaSoundHallTillHojre,
         DaSoundRetframLigeUd,
         DaSoundTa,
         DaSoundMaletErRaettfram,
         DaSoundDrejTillVenstre,
         DaSoundDrejTillHojre,
         DaSoundLavEnUVandning,
         DaSoundDuKorIDenForkerteRaktning,
         DaSoundDuErAvviketFraRutten,
         DaSoundMeddetsamme,
         DaSoundCamera,

         /* Italian */
         ItSoundLaPrima = it_base,
         ItSoundLaSeconda,
         ItSoundLaTerza,
         ItSoundLaQuarta,
         ItSoundLaQuinta,
         ItSoundLaSesta,
         ItSoundLaSettima,
         ItSoundLottava,
         ItSoundQuindi,
         ItSoundADestinazione,
         ItSoundProcedoDiritto,
         ItSoundUscita,
         ItSoundEsci,
         ItSoundHaiRaggiunto,
         ItSoundQui,
         ItSoundTra25Metri,
         ItSoundTra50Metri,
         ItSoundTra100Metri,
         ItSoundTra200Metri,
         ItSoundTra500Metri,
         ItSoundTra1Chilometro,
         ItSoundTra2Chilometri,
         ItSoundAllaRotonda,
         ItSoundMantieniLaSinistra,
         ItSoundMantieniLaDestra,
         ItSoundSvoltaASinistra,
         ItSoundSvoltaADestra,
         ItSoundSeiFuoriStrada,
         ItSoundStaiProcedendo,
         ItSoundQuandoPossibile,
         ItSoundImmediatamente,
         ItSoundPrendi,
         ItSoundEsciDallAutostrada,
         ItSoundEseguiUnInversioneAU,
         ItSoundCamera,

         /* French */
         FrSoundLaPremiere = fr_base,
         FrSoundLaDeuxieme,
         FrSoundLaTroisieme,
         FrSoundLaQuatrieme,
         FrSoundLaCinquieme,
         FrSoundLaSixieme,
         FrSoundLaSeptieme,
         FrSoundLaHuitieme,
         FrSoundImmediatement,
         FrSoundIci,
         FrSoundA25Metres,
         FrSoundA50Metres,
         FrSoundA100Metres,
         FrSoundA200Metres,
         FrSoundA500Metres,
         FrSoundA1Kilometre,
         FrSoundA2Kilometres,
         FrSoundADestination,
         FrSoundADroite,
         FrSoundAGauche,
         FrSoundAPresentVousEtes,
         FrSoundAuRondpoint,
         FrSoundConduisezToutDroit,
         FrSoundContinuez,
         FrSoundDesQuePossible,
         FrSoundFaitesDemitour,
         FrSoundPrenez,
         FrSoundQuittezLautoroute,
         FrSoundSortez,
         FrSoundSortie,
         FrSoundTournez,
         FrSoundVousEtesArrive,
         FrSoundPuis,
         FrSoundCamera,

         /* Spanish */
         EsSoundLaPrimera = es_base,
         EsSoundLaSegunda,
         EsSoundLaTercera,
         EsSoundLaCuarta,
         EsSoundLaQuinta,
         EsSoundLaSexta,
         EsSoundLaSeptima,
         EsSoundLaOctava,
         EsSoundAqui,
         EsSoundCambiaDeSentido,
         EsSoundConduceTodoRecto,
         EsSoundCuandoPuedas,
         EsSoundElDestino,
         EsSoundEn100Metros,
         EsSoundEn1Kilometro,
         EsSoundEn200Metros,
         EsSoundEn25Metros,
         EsSoundEn2Kilometros,
         EsSoundEn500Metros,
         EsSoundEn50Metros,
         EsSoundEnElDestino,
         EsSoundEnLaRotonda,
         EsSoundEstasConduciendo,
         EsSoundEstasFueraDeLaRuta,
         EsSoundGiraALaDerecha,
         EsSoundGiraALaIzquierda,
         EsSoundHasLlegadoATuDestino,
         EsSoundInmeditamente,
         EsSoundMantenteALaDerecha,
         EsSoundMantenteALaIzquierda,
         EsSoundSalida,
         EsSoundSal,
         EsSoundSalDeLaAutopista,
         EsSoundToma,
         EsSoundYDespues,
         EsSoundCamera,

         /* Dutch */
         NlSoundAfrit = nl_base,
         NlSoundBijDeBestemming,
         NlSoundDeEerste,
         NlSoundDeTweede,
         NlSoundDeDerde,
         NlSoundDeVierde,
         NlSoundDeVijfde,
         NlSoundDeZesde,
         NlSoundDeZevende,
         NlSoundDeAchtste,
         NlSoundEnDan,
         NlSoundHier,
         NlSoundKeerOmIndienMogelijk,
         NlSoundLinksAfslaan,
         NlSoundLinksHouden,
         NlSoundLinks,
         NlSoundNa100Meter,
         NlSoundNa1Kilometer,
         NlSoundNa200Meter,
         NlSoundNa25Meter,
         NlSoundNa2Kilometer,
         NlSoundNa500Meter,
         NlSoundNa50Meter,
         NlSoundNaarLinks,
         NlSoundNaarRechts,
         NlSoundOmkeren,
         NlSoundOnmiddellijk,
         NlSoundOpDeRotonde,
         NlSoundRechtdoorRijden,
         NlSoundRechtsAfslaan,
         NlSoundRechtsHouden,
         NlSoundRechts,
         NlSoundSnelwegVerlaten,
         NlSoundUBevindtZichNuBuitenDeRoute,
         NlSoundUHebtUwBestemmingBereikt,
         NlSoundURijdtInDeVerkeerdeRichting,
         NlSoundCamera,

         /* Finnish */
         FiSound25Metrin = fi_base,
         FiSound50Metrin,
         FiSound100Metrin,
         FiSound200Metrin,
         FiSound500Metrin,
         FiSound1Kilometrin,
         FiSound2Kilometrin,
         FiSoundEnsimmainen,
         FiSoundToinen,
         FiSoundKolmas,
         FiSoundNeljas,
         FiSoundViides,
         FiSoundKuudes,
         FiSoundSeitsemas,
         FiSoundKahdeksas,
         FiSoundEtSeuraaAnnettuaReittia,
         FiSoundJaSitten,
         FiSoundKaannyOikealle,
         FiSoundKaannyVasemmalle,
         FiSoundJalkeen,
         FiSoundPaassa,
         FiSoundLiikenneympyrasta,
         FiSoundOletTullutPerille,
         FiSoundPoistu,
         FiSoundPysyOikealla,
         FiSoundPysyVasemmalla,
         FiSoundTassa,
         FiSoundTasta,
         FiSoundTeeUKaannosHeti,
         FiSoundUloskaynti,
         FiSoundValitseLiikenneympyrasta,
         FiSoundValittomasti,
         FiSoundKaanny,
         FiSoundYmpari,
         FiSoundMaaranpaasiOn,
         FiSoundSuoraanEteenpain,
         FiSoundVasemmalle,
         FiSoundOikealle,
         FiSoundAjaLiikenneymparastaSouraanEteenpain,
         FiSoundCamera,
         
         /* Norwegian */
         NoSound25 = no_base,
         NoSound50,
         NoSound100,
         NoSound200,
         NoSound500,
         NoSound1,
         NoSound2,
         NoSoundForste,
         NoSoundAndre,
         NoSoundTredje,
         NoSoundFjerde,
         NoSoundFemte,
         NoSoundSjette,
         NoSoundSjuende,
         NoSoundAttende,
         NoSoundAvkjoring,
         NoSoundDestinasjonenErRettFrem,
         NoSoundDuErFremmeVedDestinasjonen,
         NoSoundDuErUtenforRuten,
         NoSoundForlatHovedveien,
         NoSoundGjorEnUSvingNarDetBlirMulig,
         NoSoundHer,
         NoSoundHold,
         NoSoundIEndenAvGaten,
         NoSoundIRundkjoringen,
         NoSoundKilometer,
         NoSoundKjorAv,
         NoSoundKjorHeleVeienRundt,
         NoSoundKjorRettFrem,
         NoSoundKjorUt,
         NoSoundMeter,
         NoSoundMiles,
         NoSoundOgSa,
         NoSoundOm,
         NoSoundSving,
         NoSoundTa,
         NoSoundTilHoyre,
         NoSoundTilVenstre,
         NoSoundUmiddelbart,
         NoSoundVedDestinasjonen,
         NoSoundVennligstSnu,
         NoSoundVenstre,
         NoSoundYards,
         NoSoundCamera,


         /* American English */
         UsSoundIn100Feet = us_base,
         UsSoundIn200Feet,
         UsSoundIn500Feet,
         UsSoundInAQuarterOfAMile,
         UsSoundInHalfAMile,
         UsSoundIn1Mile,
         UsSoundIn2Miles,
         UsSoundIn2Kilometers,
         UsSoundIn1Kilometer,
         UsSoundIn500Meters,
         UsSoundIn200Meters,
         UsSoundIn100Meters,
         UsSoundIn50Meters,
         UsSoundIn25Meters,
         UsSoundIn500Yards,
         UsSoundIn200Yards,
         UsSoundIn100Yards,
         UsSoundIn50Yards,
         UsSoundIn25Yards,
         UsSoundTheFirst,
         UsSoundTheSecond,
         UsSoundTheThird,
         UsSoundTheFourth,
         UsSoundTheFifth,
         UsSoundTheSixth,
         UsSoundTheSeventh,
         UsSoundTheEighth,
         UsSoundAndThen,
         UsSoundAtTheDestination,
         UsSoundAtTheRoundabout,
         UsSoundDriveStraightAhead,
         UsSoundExitNoun,
         UsSoundExitVerb,
         UsSoundHere,
         UsSoundImmediately,
         UsSoundKeepLeft,
         UsSoundKeepRight,
         UsSoundMakeAUTurn,
         UsSoundPleaseTurnAround,
         UsSoundTake,
         UsSoundTurnLeft,
         UsSoundTurnRight,
         UsSoundYouAreNowOffTrack,
         UsSoundYouHaveReachedYourDestination,


         UsSoundCamera,

         /* Portuguese */
         PtSound100Feet = pt_base,
         PtSound200Feet,
         PtSound500Feet,
         PtSoundAQuarterOfAMile,
         PtSoundHalfAMile,
         PtSound1Mile,
         PtSound2Miles,
         PtSound25Metros,
         PtSound50Metros,
         PtSound100Metros,
         PtSound200Metros,
         PtSound500Metros,
         PtSound1Quilometro,
         PtSound2Quilometros,
         PtSound25Yards,
         PtSound50Yards,
         PtSound100Yards,
         PtSound200Yards,
         PtSound500Yards,
         PtSoundNaPrimeira,
         PtSoundNaSegunda,
         PtSoundNaTerceira,
         PtSoundNaQuarta,
         PtSoundNaQuinta,
         PtSoundNaSexta,
         PtSoundNaSetima,
         PtSoundNaOitava,
         PtSoundA,
         PtSoundAqui,
         PtSoundDaquiA,
         PtSoundEASeguir,
         PtSoundFacaInversaoDeMarcha,
         PtSoundImediatamente,
         PtSoundMantenhaSeADireita,
         PtSoundMantenhaSeAEsquerda,
         PtSoundNaRotunda,
         PtSoundNoDestino,
         PtSoundQuandoPossivel,
         PtSoundSaia,
         PtSoundSaida,
         PtSoundSigaEmFrente,
         PtSoundVire,
         PtSoundVireADireita,
         PtSoundVireAEsquerda,
         PtSoundVoceChegouAoSeuDestino,
         PtSoundVoceEstaForaDeRota,
         PtSoundCamera,

         /* Hungarian */
         HuSound100 = hu_base,
         HuSound1,
         HuSound200,
         HuSound25,
         HuSound2,
         HuSound500,
         HuSound50,
         HuSoundAzElso,
         HuSoundAMasodik,
         HuSoundAHarmadik,
         HuSoundANegyedik,
         HuSoundAzOtodik,
         HuSoundAHatodik,
         HuSoundAHetedik,
         HuSoundANyolcadik,
         HuSoundAKijaratig,
         HuSoundAKorforgalomnal,
         HuSoundAKorforgalomban,
         HuSoundACelpontElottedVan,
         HuSoundACelpontig,
         HuSoundBalra,
         HuSoundEgyenesen,
         HuSoundElertedACelpontot,
         HuSoundFel,
         HuSoundForduljVisszaAmintLehet,
         HuSoundForduljVissza,
         HuSoundFordulj,
         HuSoundHajtsKi,
         HuSoundItt,
         HuSoundJardot,
         HuSoundJobbra,
         HuSoundKijaraton,
         HuSoundKilometert,
         HuSoundLabat,
         HuSoundMajdUtana,
         HuSoundMenjTovabbEgyenesen1,
         HuSoundMenjTovabbEgyenesen2,
         HuSoundMenj,
         HuSoundMerfoldet,
         HuSoundMetert,
         HuSoundMostLetertelAzUtvonalrol,
         HuSoundMost,
         HuSoundNegyed,
         HuSoundNyolcad,
         HuSoundRosszIranybaMesz,
         HuSoundTarts,
         HuSoundCamera,

         /* Polish */
         PlSoundWPierwszy = pl_base,
         PlSoundWDrugi,
         PlSoundWTrzeci,
         PlSoundWCzwarty,
         PlSoundWPiaty,
         PlSoundWSzusty,
         PlSoundWSiodmy,
         PlSoundWOsmy,
         PlSoundTutaj,
         PlSoundNatychmiast,
         PlSoundZa25Metrow,
         PlSoundZa50Metrow,
         PlSoundZa100Metrow,
         PlSoundZa200Metrow,
         PlSoundZa500Metrow,
         PlSoundZa25Jardow,
         PlSoundZa50Jardow,
         PlSoundZa100Jardow,
         PlSoundZa200Jardow,
         PlSoundZa500Jardow,
         PlSoundZa25Stop,
         PlSoundZa50Stop,
         PlSoundZa100Stop,
         PlSoundZa200Stop,
         PlSoundZa500Stop,
         PlSoundZa1Kilometr,
         PlSoundZa2Kilometry,
         PlSoundZaPuMili,
         PlSoundZaJednotwartoMili,
         PlSoundZaJednoossmoMili,
         PlSoundZa1Mile,
         PlSoundZa2Mile,
         PlSoundAPotem,
         PlSoundDojechales,
         PlSoundJedz,
         PlSoundJedzProsto,
         PlSoundKamera,
         PlSoundNaRondzie,
         PlSoundOstrzezenie,
         PlSoundProszeZawrocic,
         PlSoundPunkt,
         PlSoundSkrecWLewo,
         PlSoundSkrecWPrawo,
         PlSoundTeraz,
         PlSoundTrzymajSieLewej,
         PlSoundTrzymajSiePrawej,
         PlSoundWlasnie,
         PlSoundWMiejscuPrzeznaczenia,
         PlSoundZawrocGdyBedzieToMozliwe,
         PlSoundZjazd,
         PlSoundZjedz,
         PlSoundZjedzZAutostrade,
         PlSoundCamera,

         /* Czech */
         CsSoundPrvnim = cs_base,
         CsSoundDruhym,
         CsSoundTretim,
         CsSoundCtvrtym,
         CsSoundPatym,
         CsSoundSestym,
         CsSoundSedmym,
         CsSoundOsmym,
         CsSoundZde,
         CsSoundIhned,
         CsSoundPo25Metrech,
         CsSoundPo50Metrech,
         CsSoundPo100Metrech,
         CsSoundPo200Metrech,
         CsSoundPo500Metrech,
         CsSoundPo25Yardech,
         CsSoundPo50Yardech,
         CsSoundPo100Yardech,
         CsSoundPo200Yardech,
         CsSoundPo500Yardech,
         CsSoundPo25Stopach,
         CsSoundPo50Stopach,
         CsSoundPo100Stopach,
         CsSoundPo200Stopach,
         CsSoundPo500Stopach,
         CsSoundPo1Kilometru,
         CsSoundPo2Kilometrech,
         CsSoundPoPulMile,
         CsSoundPoCtvrtMile,
         CsSoundPoOsmineMile,
         CsSoundPo1Mili,
         CsSoundPo2Milich,
         CsSoundAPotom,
         CsSoundCilJePredVami,
         CsSoundDrzteSeVlevo,
         CsSoundDrzteSeVpravo,
         CsSoundJedte,
         CsSoundJedtePrimo,
         CsSoundJsteVCili,
         CsSoundNaKruhovemObjezdu,
         CsSoundNebezpeci,
         CsSoundNyniJedeteNespravnymSmerem,
         CsSoundNyniJsteMimoTrasu,
         CsSoundOdbocteVlevo,
         CsSoundOdbocteVpravo,
         CsSoundOpustteDalnici,
         CsSoundOtocteSeAzJeToMozne,
         CsSoundProsimOtocteSe,
         CsSoundRychlostniKamera,
         CsSoundVCili,
         CsSoundVyjedte,
         CsSoundVyjezdem,
         CsSoundCamera,

         /* Slovenian */
         SlSoundPrvi = sl_base,
         SlSoundDrugi,
         SlSoundTretji,
         SlSoundCetrti,
         SlSoundPeti,
         SlSoundSesti,
         SlSoundSedmi,
         SlSoundOsmi,
         SlSoundIzhod,
         SlSoundCez25Metrov,
         SlSoundCez50Metrov,
         SlSoundCez100Metrov,
         SlSoundCez200Metrov,
         SlSoundCez500Metrov,
         SlSoundCez1Kilometer,
         SlSoundCez2Kilometra,
         SlSoundInPotem,
         SlSoundIzberite,
         SlSoundKameriZaNadzorHitrosti,
         SlSoundKoBoMogoceNareditePolkrozniZavoj,
         SlSoundNaCilju,
         SlSoundNaKroznemKriziscu,
         SlSoundNemudoma,
         SlSoundOpozorilo,
         SlSoundOstaniteNaDesnemPasu,
         SlSoundOstaniteNaLevemPasu,
         SlSoundPrisliSteNaSvojCilj,
         SlSoundProsimoObrnite,
         SlSoundTukaj,
         SlSoundVasCiljJeNaravnost,
         SlSoundZapeljiteIzAvtoceste,
         SlSoundZapeljiteNaravnost,
         SlSoundZavijteDesno,
         SlSoundZavijteLevo,
         SlSoundZavijte,
         SlSoundZdajSteZasliSPoti,
//         SlSoundKroznegaKrizisca,
//         SlSoundSKroznegaKrizisca,
//         SlSoundZdajVoziteVNapacnoSmer,

      }; /* Enum Clips */

   } /* namespace AudioClipsEnum */

} /* namespace isab */

#endif /* AUDIO_CLIPS_ENUM_H */

