#ifndef __QONTEXT_SIZE_SELECTORS_HPP__
#define __QONTEXT_SIZE_SELECTORS_HPP__

namespace qtxt {
    template<int MULTIPLE = 1>
    class SquareSizeSelector {
        public:
            explicit SquareSizeSelector(int minArea = 0);
            bool operator()(int& width, int& height) const;
            SquareSizeSelector<MULTIPLE>& operator++();
            SquareSizeSelector<MULTIPLE>& operator--();
        private:
            int lowerBound, upperBound;
            int current;

            void UpdateCurrent();
    };

    class SquarePowerOfTwoSizeSelector {
        public:
            explicit SquarePowerOfTwoSizeSelector(int minArea = 0);
            bool operator()(int& width, int& height) const;
            SquarePowerOfTwoSizeSelector& operator++();
            SquarePowerOfTwoSizeSelector& operator--();
        private:
            int side;
    };

    class PowerOfTwoSizeSelector {
        public:
            explicit PowerOfTwoSizeSelector(int minArea = 0);
            bool operator()(int& width, int& height) const;
            PowerOfTwoSizeSelector& operator++();
            PowerOfTwoSizeSelector& operator--();
        private:
            int w, h;
    };
}

#endif