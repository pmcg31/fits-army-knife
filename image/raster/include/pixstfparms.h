#pragma once

namespace ELS
{

    class PixSTFParms
    {
    public:
        PixSTFParms();
        PixSTFParms(const PixSTFParms &copyFrom);

        PixSTFParms &operator=(const PixSTFParms &rhs);

        bool operator==(const PixSTFParms &rhs) const;
        bool operator!=(const PixSTFParms &rhs) const;

        double getMBal(int chan = 0) const;
        double getSClip(int chan = 0) const;
        double getHClip(int chan = 0) const;
        double getSExp(int chan = 0) const;
        double getHExp(int chan = 0) const;

        void getAll(double *mBal,
                    double *sClip,
                    double *hClip,
                    double *sExp,
                    double *hExp,
                    int chan = 0) const;

        void setMBal(double mBal, int chan = 0);
        void setSClip(double sClip, int chan = 0);
        void setHClip(double hClip, int chan = 0);
        void setSExp(double sExp, int chan = 0);
        void setHExp(double hExp, int chan = 0);

    private:
        double _mBal[3];
        double _sClip[3];
        double _hClip[3];
        double _sExp[3];
        double _hExp[3];
    };

}